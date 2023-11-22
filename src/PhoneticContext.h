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
#ifndef __PYZY_PHONETIC_CONTEXT_H_
#define __PYZY_PHONETIC_CONTEXT_H_

#include <string>
#include <vector>

#include "Config.h"
#include "Const.h"
#include "InputContext.h"
#include "PhraseEditor.h"
#include "PinyinArray.h"
#include "SpecialPhraseTable.h"
#include "Variant.h"

namespace PyZy {

#define MAX_PINYIN_LEN 64

struct Preedit {
    std::string selected_text;
    std::string candidate_text;
    std::string rest_text;

    void clear () {
        selected_text.clear ();
        candidate_text.clear ();
        rest_text.clear ();
    }
};

class PhoneticContext : public InputContext {
public:
    explicit PhoneticContext (PhoneticContext::Observer *observer);
    virtual ~PhoneticContext (void);

    /* API of InputContext */
    virtual void reset (void);

    bool selectCandidate (size_t i);
    bool focusCandidate (size_t i);
    bool focusCandidatePrevious ();
    bool focusCandidateNext ();
    bool resetCandidate (size_t i);
    bool unselectCandidates ();
    bool hasCandidate (size_t i);
    bool getCandidate (size_t i, Candidate & output);
    size_t getPreparedCandidatesSize () const;

    virtual Variant getProperty (PropertyName name) const;
    virtual bool setProperty (PropertyName name, const Variant &variant);

    /* Accessors of InputContext. */
    virtual const std::string & inputText () const
    {
        return m_text;
    }

    virtual const std::string & selectedText (void) const
    {
        return m_preedit_text.selected_text;
    }

    virtual const std::string & conversionText (void) const
    {
        return m_preedit_text.candidate_text;
    }

    virtual const std::string & restText (void) const
    {
        return m_preedit_text.rest_text;
    }

    virtual const std::string & auxiliaryText (void) const
    {
        return m_auxiliary_text;
    }

    virtual unsigned int cursor () const
    {
        return m_cursor;
    }

    virtual unsigned int focusedCandidate () const
    {
        return m_focused_candidate;
    }

protected:
    virtual void resetContext (void);
    virtual void update (void);
    virtual void commitText (const std::string & commit_text);
    virtual void updateInputText (void);
    virtual void updateCursor (void);
    virtual void updateCandidates (void);
    virtual void updateAuxiliaryText (void);
    virtual void updatePreeditText (void);
    virtual bool updateSpecialPhrases (void);

    /* inline functions */
    void updatePhraseEditor (void)
    {
        m_phrase_editor.update (m_pinyin);
    }

    const char * textAfterPinyin () const
    {
        return (const char *)m_text + m_pinyin_len;
    }

    const char * textAfterPinyin (size_t i) const
    {
        g_assert (i <= m_pinyin.size ());
        if ( G_UNLIKELY (i == 0))
            return m_text;
        i--;
        return (const char *)m_text + m_pinyin[i].begin + m_pinyin[i].len;
    }

    const char * textAfterCursor () const
    {
        return (const char *)m_text + m_cursor;
    }

    /* variables */
    Config                      m_config;
    size_t                      m_cursor;
    size_t                      m_focused_candidate;
    PinyinArray                 m_pinyin;
    size_t                      m_pinyin_len;
    String                      m_buffer;
    PhraseEditor                m_phrase_editor;
    std::vector<std::string>    m_special_phrases;
    std::string                 m_selected_special_phrase;
    String                      m_text;
    Preedit                     m_preedit_text;
    std::string                 m_auxiliary_text;

private:
    PhoneticContext::Observer  *m_observer;
};

}; // namespace PyZy

#endif  // __PYZY_PHONETIC_CONTEXT_H_
