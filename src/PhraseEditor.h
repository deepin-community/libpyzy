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
#ifndef __PYZY_PHRASE_EDITOR_H_
#define __PYZY_PHRASE_EDITOR_H_

#include "PhraseArray.h"
#include "PinyinArray.h"
#include "String.h"
#include "Util.h"

#define FILL_GRAN (12)

namespace PyZy {

class Config;
class Database;
class Query;

class PhraseEditor {
public:
    explicit PhraseEditor (const Config & config);
    ~PhraseEditor (void);

    const String & selectedString (void) const  { return m_selected_string; }
    const PinyinArray & pinyin (void) const     { return m_pinyin; }
    const PhraseArray & candidates (void) const { return m_candidates; }
    size_t cursor (void) const                   { return m_cursor; }

    size_t cursorInChar (void) const
    {
        return m_cursor == 0 ? 0 : m_pinyin[m_cursor - 1].begin + m_pinyin[m_cursor - 1].len;
    }

    bool pinyinExistsAfterCursor (void) const
    {
        return m_pinyin.size () > m_cursor;
    }

    const Phrase & candidate (size_t i) const
    {
        return m_candidates[i];
    }

    bool fillCandidates (void);

    const PhraseArray & candidate0 (void) const
    {
        return m_candidate_0_phrases;
    }

    bool candidateIsUserPhrase (size_t i) const
    {
        const Phrase & phrase = m_candidates[i];
        return phrase.len > 1 && phrase.user_freq > 0 && phrase.freq == 0;
    }

    bool unselectCandidates (void)
    {
        if (m_cursor == 0) {
            return false;
        }
        else {
            m_selected_phrases.clear ();
            m_selected_string.truncate (0);
            m_cursor = 0;
            updateCandidates ();
            return true;
        }
    }

    void reset (void)
    {
        m_candidates.clear ();
        m_selected_phrases.clear ();
        m_selected_string.truncate (0);
        m_candidate_0_phrases.clear ();
        m_pinyin.clear ();
        m_cursor = 0;
        m_query.reset ();
    }

    bool update (const PinyinArray &pinyin);
    bool selectCandidate (size_t i);
    bool resetCandidate (size_t i);
    void commit (void);

    bool empty (void) const
    {
        return m_selected_string.empty () && m_candidate_0_phrases.empty ();
    }

    operator bool (void) const
    {
        return !empty ();
    }

private:
    void updateCandidates (void);
    void updateTheFirstCandidate (void);

private:
    const Config &m_config;
    PhraseArray m_candidates;           // candidates phrase array
    PhraseArray m_selected_phrases;     // selected phrases, before cursor
    String      m_selected_string;      // selected phrases, in string format
    PhraseArray m_candidate_0_phrases;  // the first candidate in phrase array format
    PinyinArray m_pinyin;
    size_t m_cursor;
    std::shared_ptr<Query> m_query;
};

};  // namespace PyZy

#endif  // __PYZY_PHRASE_EDITOR_H_
