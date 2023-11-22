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
#include "SpecialPhraseTable.h"

#include <fstream>

#include "DynamicSpecialPhrase.h"
#include "SpecialPhrase.h"

namespace PyZy {

std::unique_ptr<SpecialPhraseTable> SpecialPhraseTable::m_instance;

class StaticSpecialPhrase : public SpecialPhrase {
public:
    StaticSpecialPhrase (const std::string &text, size_t pos) :
        SpecialPhrase (pos), m_text (text) { }
    ~StaticSpecialPhrase (void) { }

    std::string text (void) { return m_text; }

private:
    std::string m_text;
};

SpecialPhraseTable::SpecialPhraseTable (const std::string &config_dir)
{
    char * path =
        g_build_filename (config_dir.c_str(), "phrases.txt", NULL);

    load ("phrases.txt") ||
    load (path) ||
    load (PKGDATADIR G_DIR_SEPARATOR_S "phrases.txt");
    g_free (path);
}

bool
SpecialPhraseTable::lookup (const std::string         &command,
                            std::vector<std::string>  &result)
{
    result.clear ();

    std::pair<Map::iterator, Map::iterator> range = m_map.equal_range (command);
    for (Map::iterator it = range.first; it != range.second; it ++) {
        result.push_back ((*it).second->text ());
    }

    return result.size () > 0;
}

bool
SpecialPhraseTable::load (const char *file)
{
    m_map.clear ();

    std::ifstream in (file);
    if (in.fail ())
        return false;

    std::string line;
    while (!in.eof ()) {
        getline (in, line);
        if (line.size () == 0 || line[0] == ';')
            continue;
        size_t pos = line.find ('=');
        if (pos == line.npos)
            continue;

        std::string command = line.substr(0, pos);
        std::string value = line.substr(pos + 1);
        if (command.empty () || value.empty ())
            continue;

        if (value[0] != '#') {
            SpecialPhrasePtr phrase (new StaticSpecialPhrase (value, 0));
            m_map.insert (Map::value_type (command, phrase));
        }
        else if (value.size () > 1) {
            SpecialPhrasePtr phrase (new DynamicSpecialPhrase (value.substr (1), 0));
            m_map.insert (Map::value_type (command, phrase));
        }
    }
    return true;
}

void
SpecialPhraseTable::init (const std::string &config_dir)
{
    if (config_dir.empty ()) {
        g_error ("Error: An argument of init is empty string.");
        return;
    }
    m_instance.reset (new SpecialPhraseTable (config_dir));
}

SpecialPhraseTable &
SpecialPhraseTable::instance (void)
{
    if (m_instance.get () == NULL) {
        g_error ("Error: Please call PyZy::InputContext::init () !");
    }
    return *m_instance;
}


};  // namespace PyZy
