/* vim:set et ts=4 sts=4:
 *
 * libpyzy - The Chinese PinYin and Bopomofo conversion library.
 *
 * Copyright (c) 2008-2010 Peng Huang <shawn.p.huang@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
 * USA
 */
#include "Database.h"

#include <glib.h>
#include <glib/gstdio.h>
#include <sqlite3.h>

#include "Config.h"
#include "PinyinArray.h"
#include "Util.h"


namespace PyZy {

#define DB_CACHE_SIZE       "5000"
#define DB_INDEX_SIZE       (3)
/* define columns */
#define DB_COLUMN_USER_FREQ (0)
#define DB_COLUMN_PHRASE    (1)
#define DB_COLUMN_FREQ      (2)
#define DB_COLUMN_S0        (3)

#define DB_PREFETCH_LEN     (6)
#define DB_BACKUP_TIMEOUT   (60)

#define USER_DICTIONARY_FILE  "user-1.0.db"


std::unique_ptr<Database> Database::m_instance;

class Conditions : public std::vector<std::string> {
public:
    Conditions (void) : std::vector<std::string> (1) {}

    void double_ (void) {
        size_t i = size ();
        // To avoid a invalid referece caused by a memory reallocation, which
        // may occur on push_back, we call reserve here.
        reserve (i * 2);
        do {
            push_back (at (--i));
        } while (i > 0);
    }

    void triple (void) {
        size_t i = size ();
        // To avoid a invalid referece caused by a memory reallocation, which
        // may occur on push_back, we call reserve here.
        reserve (i * 3);
        do {
            const std::string & value = at (--i);
            push_back (value);
            push_back (value);
        } while (i > 0);
    }

    void appendVPrintf (size_t begin, size_t end, const char *fmt, va_list args) {
        char str[64];
        g_vsnprintf (str, sizeof(str), fmt, args);
        for (size_t i = begin; i < end; i++) {
            at (i) += str;
        }
    }

    void appendPrintf (size_t begin, size_t end, const char *fmt, ...) {
        va_list args;
        va_start (args, fmt);
        appendVPrintf (begin, end, fmt, args);
        va_end (args);
    }
};

class SQLStmt {
public:
    SQLStmt (sqlite3 *db)
        : m_db (db), m_stmt (NULL) {
        g_assert (m_db != NULL);
    }

    ~SQLStmt () {
        if (m_stmt != NULL) {
            if (sqlite3_finalize (m_stmt) != SQLITE_OK) {
                g_warning ("destroy sqlite stmt failed!");
            }
        }
    }

    bool prepare (const String &sql) {
        if (sqlite3_prepare (m_db,
                             sql.c_str (),
                             sql.size (),
                             &m_stmt,
                             NULL) != SQLITE_OK) {
            g_warning ("parse sql failed!\n %s", sql.c_str ());
            return false;
        }

        return true;
    }

    bool step (void) {
        switch (sqlite3_step (m_stmt)) {
        case SQLITE_ROW:
            return true;
        case SQLITE_DONE:
            return false;
        default:
            g_warning ("sqlites step error!");
            return false;
        }
    }

    const char *columnText (int col) {
        return (const char *) sqlite3_column_text (m_stmt, col);
    }

    int columnInt (int col) {
        return sqlite3_column_int (m_stmt, col);
    }

private:
    sqlite3 *m_db;
    sqlite3_stmt *m_stmt;
};

Query::Query (const PinyinArray    & pinyin,
              size_t                 pinyin_begin,
              size_t                 pinyin_len,
              unsigned int           option)
    : m_pinyin (pinyin),
      m_pinyin_begin (pinyin_begin),
      m_pinyin_len (pinyin_len),
      m_option (option)
{
    g_assert (m_pinyin.size () >= pinyin_begin + pinyin_len);
}

Query::~Query (void)
{
}

int
Query::fill (PhraseArray &phrases, int count)
{
    int row = 0;

    while (m_pinyin_len > 0) {
        if (G_LIKELY (m_stmt.get () == NULL)) {
            m_stmt = Database::instance ().query (m_pinyin, m_pinyin_begin, m_pinyin_len, -1, m_option);
            g_assert (m_stmt.get () != NULL);
        }

        while (m_stmt->step ()) {
            Phrase phrase;

            g_strlcpy (phrase.phrase,
                       m_stmt->columnText (DB_COLUMN_PHRASE),
                       sizeof (phrase.phrase));
            phrase.freq = m_stmt->columnInt (DB_COLUMN_FREQ);
            phrase.user_freq = m_stmt->columnInt (DB_COLUMN_USER_FREQ);
            phrase.len = m_pinyin_len;

            for (size_t i = 0, column = DB_COLUMN_S0; i < m_pinyin_len; i++) {
                phrase.pinyin_id[i].sheng = m_stmt->columnInt (column++);
                phrase.pinyin_id[i].yun = m_stmt->columnInt (column++);
            }

            phrases.push_back (phrase);
            row ++;
            if (G_UNLIKELY (row == count)) {
                return row;
            }
        }

        m_stmt.reset ();
        m_pinyin_len --;
    }

    return row;
}

Database::Database (const std::string &user_data_dir)
    : m_db (NULL)
    , m_timeout_id (0)
    , m_timer (g_timer_new ())
    , m_user_data_dir (user_data_dir)
{
    open ();
}

Database::~Database (void)
{
    g_timer_destroy (m_timer);
    if (m_timeout_id != 0) {
        saveUserDB ();
        g_source_remove (m_timeout_id);
    }
    if (m_db) {
        if (sqlite3_close (m_db) != SQLITE_OK) {
            g_warning ("close sqlite database failed!");
        }
    }
}

inline bool
Database::executeSQL (const char *sql, sqlite3 *db)
{
    if (db == NULL)
        db = m_db;

    char *errmsg = NULL;
    if (sqlite3_exec (db, sql, NULL, NULL, &errmsg) != SQLITE_OK) {
        g_warning ("%s: %s", errmsg, sql);
        sqlite3_free (errmsg);
        return false;
    }
    return true;
}

bool
Database::open (void)
{
    do {
#if (SQLITE_VERSION_NUMBER >= 3006000)
        sqlite3_initialize ();
#endif
        static const char * maindb [] = {
            PKGDATADIR"/db/local.db",
            PKGDATADIR"/db/open-phrase.db",
            PKGDATADIR"/db/android.db",
            "main.db",
        };

        size_t i;
        for (i = 0; i < G_N_ELEMENTS (maindb); i++) {
            if (!g_file_test(maindb[i], G_FILE_TEST_IS_REGULAR))
                continue;
            if (sqlite3_open_v2 (maindb[i], &m_db,
                SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL) == SQLITE_OK) {
                break;
            }
        }

        if (i == G_N_ELEMENTS (maindb)) {
            g_warning ("can not open main database");
            break;
        }

        m_sql.clear ();

        /* Set synchronous=OFF, write user database will become much faster.
         * It will cause user database corrupted, if the operatering system
         * crashes or computer loses power.
         * */
        m_sql << "PRAGMA synchronous=OFF;\n";

        /* Set the cache size for better performance */
        m_sql << "PRAGMA cache_size=" DB_CACHE_SIZE ";\n";

        /* Using memory for temp store */
        // m_sql << "PRAGMA temp_store=MEMORY;\n";

        /* Set journal mode */
        // m_sql << "PRAGMA journal_mode=PERSIST;\n";

        /* Using EXCLUSIVE locking mode on databases
         * for better performance */
        m_sql << "PRAGMA locking_mode=EXCLUSIVE;\n";
        if (!executeSQL (m_sql))
            break;

        loadUserDB ();
#if 0
    /* Attach user database */

    g_mkdir_with_parents (m_user_data_dir, 0750);
    m_buffer.clear ();
    m_buffer << m_user_data_dir << G_DIR_SEPARATOR_S << USER_DICTIONARY_FILE;

    retval = openUserDB (m_buffer);
    if (!retval) {
        g_warning ("Can not open user database %s", m_buffer.c_str ());
        if (!openUserDB (":memory:"))
            goto _failed;
    }
#endif

        /* prefetch some tables */
        // prefetch ();

        return true;
    } while (0);

    if (m_db) {
        sqlite3_close (m_db);
        m_db = NULL;
    }
    return false;
}

bool
Database::loadUserDB (void)
{
    sqlite3 *userdb = NULL;
    do {
        /* Attach user database */
        m_sql.printf ("ATTACH DATABASE \":memory:\" AS userdb;");
        if (!executeSQL (m_sql))
            break;

        g_mkdir_with_parents (m_user_data_dir, 0750);
        m_buffer.clear ();
        m_buffer << m_user_data_dir << G_DIR_SEPARATOR_S << USER_DICTIONARY_FILE;

        unsigned int flags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE;
        if (sqlite3_open_v2 (m_buffer, &userdb, flags, NULL) != SQLITE_OK &&
            sqlite3_open_v2 (":memory:", &userdb, flags, NULL) != SQLITE_OK)
            break;

        m_sql = "BEGIN TRANSACTION;\n";
        /* create desc table*/
        m_sql << "CREATE TABLE IF NOT EXISTS desc (name PRIMARY KEY, value TEXT);\n";
        m_sql << "INSERT OR IGNORE INTO desc VALUES " << "('version', '1.2.0');\n"
              << "INSERT OR IGNORE INTO desc VALUES " << "('uuid', '" << UUID () << "');\n"
              << "INSERT OR IGNORE INTO desc VALUES " << "('hostname', '" << Hostname () << "');\n"
              << "INSERT OR IGNORE INTO desc VALUES " << "('username', '" << Env ("USERNAME") << "');\n"
              << "INSERT OR IGNORE INTO desc VALUES " << "('create-time', datetime());\n"
              << "INSERT OR IGNORE INTO desc VALUES " << "('attach-time', datetime());\n";

        /* create phrase tables */
        for (size_t i = 0; i < MAX_PHRASE_LEN; i++) {
            m_sql.appendPrintf ("CREATE TABLE IF NOT EXISTS py_phrase_%d (user_freq, phrase TEXT, freq INTEGER ", i);
            for (size_t j = 0; j <= i; j++)
                m_sql.appendPrintf (",s%d INTEGER, y%d INTEGER", j, j);
            m_sql << ");\n";
        }

        /* create index */
        m_sql << "CREATE UNIQUE INDEX IF NOT EXISTS " << "index_0_0 ON py_phrase_0(s0,y0,phrase);\n";
        m_sql << "CREATE UNIQUE INDEX IF NOT EXISTS " << "index_1_0 ON py_phrase_1(s0,y0,s1,y1,phrase);\n";
        m_sql << "CREATE INDEX IF NOT EXISTS " << "index_1_1 ON py_phrase_1(s0,s1,y1);\n";
        for (size_t i = 2; i < MAX_PHRASE_LEN; i++) {
            m_sql << "CREATE UNIQUE INDEX IF NOT EXISTS " << "index_" << i << "_0 ON py_phrase_" << i
                  << "(s0,y0";
            for (size_t j = 1; j <= i; j++)
                m_sql << ",s" << j << ",y" << j;
            m_sql << ",phrase);\n";
            m_sql << "CREATE INDEX IF NOT EXISTS " << "index_" << i << "_1 ON py_phrase_" << i << "(s0,s1,s2,y2);\n";
        }
        m_sql << "COMMIT;";

        if (!executeSQL (m_sql, userdb))
            break;

        sqlite3_backup *backup = sqlite3_backup_init (m_db, "userdb", userdb, "main");

        if (backup) {
            sqlite3_backup_step (backup, -1);
            sqlite3_backup_finish (backup);
        }

        sqlite3_close (userdb);
        return true;
    } while (0);

    if (userdb)
        sqlite3_close (userdb);
    return false;
}

bool
Database::saveUserDB (void)
{
    g_mkdir_with_parents (m_user_data_dir, 0750);
    m_buffer.clear ();
    m_buffer << m_user_data_dir << G_DIR_SEPARATOR_S << USER_DICTIONARY_FILE;

    String tmpfile = m_buffer + "-tmp";
    sqlite3 *userdb = NULL;
    do {
        /* remove tmpfile if it exist */
        g_unlink (tmpfile);

        unsigned int flags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE;
        if (sqlite3_open_v2 (tmpfile, &userdb, flags, NULL) != SQLITE_OK)
            break;

        sqlite3_backup *backup = sqlite3_backup_init (userdb, "main", m_db, "userdb");

        if (backup == NULL)
            break;

        sqlite3_backup_step (backup, -1);
        sqlite3_backup_finish (backup);
        sqlite3_close (userdb);

        g_rename (tmpfile, m_buffer);

        return true;
    } while (0);

    if (userdb != NULL)
        sqlite3_close (userdb);
    g_unlink (tmpfile);

    return false;
}

void
Database::prefetch (void)
{
    m_sql.clear ();
    for (size_t i = 0; i < DB_PREFETCH_LEN; i++)
        m_sql << "SELECT * FROM py_phrase_" << i << ";\n";

    // g_debug ("prefetching ...");
    executeSQL (m_sql);
    // g_debug ("done");
}

// This function should be return gboolean because g_timeout_add_seconds requires it.
gboolean
Database::timeoutCallback (void * data)
{
    Database *self = static_cast<Database*> (data);

    /* Get elapsed time since last modification of database. */
    unsigned int elapsed = (unsigned int)g_timer_elapsed (self->m_timer, NULL);

    if (elapsed >= DB_BACKUP_TIMEOUT &&
        self->saveUserDB ()) {
        self->m_timeout_id = 0;
        return false;
    }

    return true;
}

void
Database::modified (void)
{
    /* Restart the timer */
    g_timer_start (m_timer);

    if (m_timeout_id != 0)
        return;

    m_timeout_id = g_timeout_add_seconds (DB_BACKUP_TIMEOUT,
                                          Database::timeoutCallback,
                                          static_cast<void *> (this));
}

inline static bool
pinyin_option_check_sheng (unsigned int option, unsigned int id, unsigned int fid)
{
    switch ((id << 16) | fid) {
    case (PINYIN_ID_C << 16) | PINYIN_ID_CH:
        return (option & PINYIN_FUZZY_C_CH);
    case (PINYIN_ID_CH << 16) | PINYIN_ID_C:
        return (option & PINYIN_FUZZY_CH_C);
    case (PINYIN_ID_Z << 16) | PINYIN_ID_ZH:
        return (option & PINYIN_FUZZY_Z_ZH);
    case (PINYIN_ID_ZH << 16) | PINYIN_ID_Z:
        return (option & PINYIN_FUZZY_ZH_Z);
    case (PINYIN_ID_S << 16) | PINYIN_ID_SH:
        return (option & PINYIN_FUZZY_S_SH);
    case (PINYIN_ID_SH << 16) | PINYIN_ID_S:
        return (option & PINYIN_FUZZY_SH_S);
    case (PINYIN_ID_L << 16) | PINYIN_ID_N:
        return (option & PINYIN_FUZZY_L_N);
    case (PINYIN_ID_N << 16) | PINYIN_ID_L:
        return (option & PINYIN_FUZZY_N_L);
    case (PINYIN_ID_F << 16) | PINYIN_ID_H:
        return (option & PINYIN_FUZZY_F_H);
    case (PINYIN_ID_H << 16) | PINYIN_ID_F:
        return (option & PINYIN_FUZZY_H_F);
    case (PINYIN_ID_L << 16) | PINYIN_ID_R:
        return (option & PINYIN_FUZZY_L_R);
    case (PINYIN_ID_R << 16) | PINYIN_ID_L:
        return (option & PINYIN_FUZZY_R_L);
    case (PINYIN_ID_K << 16) | PINYIN_ID_G:
        return (option & PINYIN_FUZZY_K_G);
    case (PINYIN_ID_G << 16) | PINYIN_ID_K:
        return (option & PINYIN_FUZZY_G_K);
    default: return false;
    }
}

inline static bool
pinyin_option_check_yun (unsigned int option, unsigned int id, unsigned int fid)
{
    switch ((id << 16) | fid) {
    case (PINYIN_ID_AN << 16) | PINYIN_ID_ANG:
        return (option & PINYIN_FUZZY_AN_ANG);
    case (PINYIN_ID_ANG << 16) | PINYIN_ID_AN:
        return (option & PINYIN_FUZZY_ANG_AN);
    case (PINYIN_ID_EN << 16) | PINYIN_ID_ENG:
        return (option & PINYIN_FUZZY_EN_ENG);
    case (PINYIN_ID_ENG << 16) | PINYIN_ID_EN:
        return (option & PINYIN_FUZZY_ENG_EN);
    case (PINYIN_ID_IN << 16) | PINYIN_ID_ING:
        return (option & PINYIN_FUZZY_IN_ING);
    case (PINYIN_ID_ING << 16) | PINYIN_ID_IN:
        return (option & PINYIN_FUZZY_ING_IN);
    case (PINYIN_ID_IAN << 16) | PINYIN_ID_IANG:
        return (option & PINYIN_FUZZY_IAN_IANG);
    case (PINYIN_ID_IANG << 16) | PINYIN_ID_IAN:
        return (option & PINYIN_FUZZY_IANG_IAN);
    case (PINYIN_ID_UAN << 16) | PINYIN_ID_UANG:
        return (option & PINYIN_FUZZY_UAN_UANG);
    case (PINYIN_ID_UANG << 16) | PINYIN_ID_UAN:
        return (option & PINYIN_FUZZY_UANG_UAN);
    default: return false;
    }
}

SQLStmtPtr
Database::query (const PinyinArray &pinyin,
                 size_t             pinyin_begin,
                 size_t             pinyin_len,
                 int                m,
                 unsigned int       option)
{
    g_assert (pinyin_begin < pinyin.size ());
    g_assert (pinyin_len <= pinyin.size () - pinyin_begin);
    g_assert (pinyin_len <= MAX_PHRASE_LEN);

    /* prepare sql */
    Conditions conditions;

    for (size_t i = 0; i < pinyin_len; i++) {
        const Pinyin *p;
        bool fs1, fs2;
        p = pinyin[i + pinyin_begin];

        fs1 = pinyin_option_check_sheng (option, p->pinyin_id[0].sheng, p->pinyin_id[1].sheng);
        fs2 = pinyin_option_check_sheng (option, p->pinyin_id[0].sheng, p->pinyin_id[2].sheng);

        if (G_LIKELY (i > 0))
            conditions.appendPrintf (0, conditions.size (),
                                       " AND ");

        if (G_UNLIKELY (fs1 || fs2)) {
            if (G_LIKELY (i < DB_INDEX_SIZE)) {
                if (fs1 && fs2 == 0) {
                    conditions.double_ ();
                    conditions.appendPrintf (0, conditions.size ()  >> 1,
                                               "s%d=%d", i, p->pinyin_id[0].sheng);
                    conditions.appendPrintf (conditions.size () >> 1, conditions.size (),
                                               "s%d=%d", i, p->pinyin_id[1].sheng);
                }
                else if (fs1 == 0 && fs2) {
                    conditions.double_ ();
                    conditions.appendPrintf (0, conditions.size ()  >> 1,
                                               "s%d=%d", i, p->pinyin_id[0].sheng);
                    conditions.appendPrintf (conditions.size () >> 1, conditions.size (),
                                               "s%d=%d", i, p->pinyin_id[2].sheng);
                }
                else {
                    size_t len = conditions.size ();
                    conditions.triple ();
                    conditions.appendPrintf (0, len,
                                               "s%d=%d", i, p->pinyin_id[0].sheng);
                    conditions.appendPrintf (len, len << 1,
                                               "s%d=%d", i, p->pinyin_id[1].sheng);
                    conditions.appendPrintf (len << 1, conditions.size (),
                                               "s%d=%d", i, p->pinyin_id[2].sheng);
                }
            }
            else {
                if (fs1 && fs2 == 0) {
                    conditions.appendPrintf (0, conditions.size (),
                                               "s%d IN (%d,%d)", i, p->pinyin_id[0].sheng, p->pinyin_id[1].sheng);
                }
                else if (fs1 == 0 && fs2) {
                    conditions.appendPrintf (0, conditions.size (),
                                               "s%d IN (%d,%d)", i, p->pinyin_id[0].sheng, p->pinyin_id[2].sheng);
                }
                else {
                    conditions.appendPrintf (0, conditions.size (),
                                               "s%d IN (%d,%d,%d)", i, p->pinyin_id[0].sheng, p->pinyin_id[1].sheng, p->pinyin_id[2].sheng);
                }
            }
        }
        else {
            conditions.appendPrintf (0, conditions.size (),
                                       "s%d=%d", i, p->pinyin_id[0].sheng);
        }

        if (p->pinyin_id[0].yun != PINYIN_ID_ZERO) {
            if (pinyin_option_check_yun (option, p->pinyin_id[0].yun, p->pinyin_id[1].yun)) {
                if (G_LIKELY (i < DB_INDEX_SIZE)) {
                    conditions.double_ ();
                    conditions.appendPrintf (0, conditions.size ()  >> 1,
                                               " AND y%d=%d", i, p->pinyin_id[0].yun);
                    conditions.appendPrintf (conditions.size () >> 1, conditions.size (),
                                               " and y%d=%d", i, p->pinyin_id[1].yun);
                }
                else {
                    conditions.appendPrintf (0, conditions.size (),
                                               " AND y%d IN (%d,%d)", i, p->pinyin_id[0].yun, p->pinyin_id[1].yun);
                }
            }
            else {
                conditions.appendPrintf (0, conditions.size (),
                                           " AND y%d=%d", i, p->pinyin_id[0].yun);
            }
        }
    }


    m_buffer.clear ();
    for (size_t i = 0; i < conditions.size (); i++) {
        if (G_UNLIKELY (i == 0))
            m_buffer << "  (" << conditions[i] << ")\n";
        else
            m_buffer << "  OR (" << conditions[i] << ")\n";
    }

    m_sql.clear ();
    int id = pinyin_len - 1;
    m_sql << "SELECT * FROM ("
                "SELECT 0 AS user_freq, * FROM main.py_phrase_" << id << " WHERE " << m_buffer << " UNION ALL "
                "SELECT * FROM userdb.py_phrase_" << id << " WHERE " << m_buffer << ") "
                    "GROUP BY phrase ORDER BY user_freq DESC, freq DESC";
    if (m > 0)
        m_sql << " LIMIT " << m;
#if 0
    g_debug ("sql =\n%s", m_sql.c_str ());
#endif

    /* query database */
    SQLStmtPtr stmt (new SQLStmt (m_db));

    if (!stmt->prepare (m_sql)) {
        stmt.reset ();
    }

    return stmt;
}

inline void
Database::phraseWhereSql (const Phrase & p, String & sql)
{
    sql << " WHERE";
    sql << " s0=" << p.pinyin_id[0].sheng
        << " AND y0=" << p.pinyin_id[0].yun;
    for (size_t i = 1; i < p.len; i++) {
        sql << " AND s" << i << '=' << p.pinyin_id[i].sheng
            << " AND y" << i << '=' << p.pinyin_id[i].yun;
    }
    sql << " AND phrase=\"" << p.phrase << "\"";

}

inline void
Database::phraseSql (const Phrase & p, String & sql)
{
    sql << "INSERT OR IGNORE INTO userdb.py_phrase_" << p.len - 1
        << " VALUES(" << 0                  /* user_freq */
        << ",\"" << p.phrase << '"'         /* phrase */
        << ','   << p.freq;                 /* freq */

    for (size_t i = 0; i < p.len; i++) {
        sql << ',' << p.pinyin_id[i].sheng << ',' << p.pinyin_id[i].yun;
    }

    sql << ");\n";

    sql << "UPDATE userdb.py_phrase_" << p.len - 1
        << " SET user_freq=user_freq+1";

    phraseWhereSql (p, sql);
    sql << ";\n";
}

void
Database::commit (const PhraseArray  &phrases)
{
    Phrase phrase = {""};

    m_sql = "BEGIN TRANSACTION;\n";
    for (size_t i = 0; i < phrases.size (); i++) {
        phrase += phrases[i];
        phraseSql (phrases[i], m_sql);
    }
    if (phrases.size () > 1)
        phraseSql (phrase, m_sql);
    m_sql << "COMMIT;\n";

    executeSQL (m_sql);
    modified ();
}

void
Database::remove (const Phrase & phrase)
{
    m_sql = "BEGIN TRANSACTION;\n";
    m_sql << "DELETE FROM userdb.py_phrase_" << phrase.len - 1;
    phraseWhereSql (phrase, m_sql);
    m_sql << ";\n";
    m_sql << "COMMIT;\n";

    executeSQL (m_sql);
    modified ();
}

void
Database::init (const std::string & user_data_dir)
{
    if (m_instance.get () == NULL) {
        m_instance.reset (new Database (user_data_dir));
    }
}

void
Database::finalize (void)
{
    m_instance.reset (NULL);
}

};  // namespace PyZy
