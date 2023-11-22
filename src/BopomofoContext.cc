/* vim:set et ts=4 sts=4:
 *
 * libpyzy - The Chinese PinYin and Bopomofo conversion library.
 *
 * Copyright (c) 2008-2010 Peng Huang <shawn.p.huang@gmail.com>
 * Copyright (c) 2010 BYVoid <byvoid1@gmail.com>
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
#include "BopomofoContext.h"

#include "Config.h"
#include "PinyinParser.h"
#include "SimpTradConverter.h"

namespace PyZy {
#include "BopomofoKeyboard.h"

BopomofoContext::BopomofoContext (PhoneticContext::Observer *observer)
    : PhoneticContext (observer),
      m_bopomofo_schema (BOPOMOFO_KEYBOARD_STANDARD)
{
}

BopomofoContext::~BopomofoContext (void)
{
}

bool
BopomofoContext::insert (char ch)
{
    if (keyvalToBopomofo (ch) == BOPOMOFO_ZERO) {
        return false;
    }

    /* is full */
    if (G_UNLIKELY (m_text.length () >= MAX_PINYIN_LEN))
        return true;

    m_text.insert (m_cursor++, ch);
    updateInputText ();
    updateCursor ();

    if (G_UNLIKELY (!(m_config.option & PINYIN_INCOMPLETE_PINYIN))) {
        updateSpecialPhrases ();
        updatePinyin ();
    }
    else if (G_LIKELY (m_cursor <= m_pinyin_len + 2)) {
        updateSpecialPhrases ();
        updatePinyin ();
    }
    else {
        if (updateSpecialPhrases ()) {
            update ();
        }
        else {
            updatePreeditText ();
            updateAuxiliaryText ();
        }
    }

    return true;
}

bool
BopomofoContext::removeCharBefore (void)
{
    if (G_UNLIKELY (m_cursor == 0))
        return false;

    m_cursor --;
    m_text.erase (m_cursor, 1);
    updateInputText ();
    updateCursor ();
    updateSpecialPhrases ();
    updatePinyin ();

    return true;
}

bool
BopomofoContext::removeCharAfter (void)
{
    if (G_UNLIKELY (m_cursor == m_text.length ()))
        return false;

    m_text.erase (m_cursor, 1);
    updateInputText ();
    updatePreeditText ();
    updateAuxiliaryText ();

    return true;
}

bool
BopomofoContext::removeWordBefore (void)
{
    if (G_UNLIKELY (m_cursor == 0))
        return false;

    size_t cursor;

    if (G_UNLIKELY (m_cursor > m_pinyin_len)) {
        cursor = m_pinyin_len;
    }
    else {
        const Pinyin & p = *m_pinyin.back ();
        cursor = m_cursor - p.len;
        m_pinyin_len -= p.len;
        m_pinyin.pop_back ();
    }

    m_text.erase (cursor, m_cursor - cursor);
    m_cursor = cursor;
    updateInputText ();
    updateCursor ();
    updateSpecialPhrases ();
    updatePhraseEditor ();
    update ();
    return true;
}

bool
BopomofoContext::removeWordAfter (void)
{
    if (G_UNLIKELY (m_cursor == m_text.length ()))
        return false;

    m_text.erase (m_cursor, -1);
    updateInputText ();
    updatePreeditText ();
    updateAuxiliaryText ();
    return true;
}

bool
BopomofoContext::moveCursorLeft (void)
{
    if (G_UNLIKELY (m_cursor == 0))
        return false;

    m_cursor --;
    updateCursor ();
    updateSpecialPhrases ();
    updatePinyin ();

    return true;
}

bool
BopomofoContext::moveCursorRight (void)
{
    if (G_UNLIKELY (m_cursor == m_text.length ()))
        return false;

    m_cursor ++;
    updateCursor ();
    updateSpecialPhrases ();
    updatePinyin ();

    return true;
}

bool
BopomofoContext::moveCursorLeftByWord (void)
{
    if (G_UNLIKELY (m_cursor == 0))
        return false;

    if (G_UNLIKELY (m_cursor > m_pinyin_len)) {
        m_cursor = m_pinyin_len;
        return true;
    }

    const Pinyin & p = *m_pinyin.back ();
    m_cursor -= p.len;
    m_pinyin_len -= p.len;
    m_pinyin.pop_back ();

    updateCursor ();
    updateSpecialPhrases ();
    updatePhraseEditor ();
    update ();

    return true;
}

bool
BopomofoContext::moveCursorRightByWord (void)
{
    return moveCursorToEnd ();
}

bool
BopomofoContext::moveCursorToBegin (void)
{
    if (G_UNLIKELY (m_cursor == 0))
        return false;

    m_cursor = 0;
    m_pinyin.clear ();
    m_pinyin_len = 0;

    updateCursor ();
    updateSpecialPhrases ();
    updatePhraseEditor ();
    update ();

    return true;
}

bool
BopomofoContext::moveCursorToEnd (void)
{
    if (G_UNLIKELY (m_cursor == m_text.length ()))
        return false;

    m_cursor = m_text.length ();
    updateCursor ();
    updateSpecialPhrases ();
    updatePinyin ();

    return true;
}

void
BopomofoContext::updatePinyin (void)
{
    if (G_UNLIKELY (m_text.empty ())) {
        m_pinyin.clear ();
        m_pinyin_len = 0;
    }
    else {
        std::wstring bopomofo;
        for(String::iterator i = m_text.begin (); i != m_text.end (); ++i) {
            bopomofo += bopomofo_char[keyvalToBopomofo (*i)];
        }

        m_pinyin_len = PinyinParser::parseBopomofo (
            bopomofo,            // bopomofo
            m_cursor,            // text length
            m_config.option,     // option
            m_pinyin,            // result
            MAX_PHRASE_LEN);     // max result length
    }

    updatePhraseEditor ();
    update ();
}

void
BopomofoContext::updateAuxiliaryText (void)
{
    if (G_UNLIKELY (m_text.empty () || !hasCandidate (0))) {
        m_auxiliary_text = "";
        PhoneticContext::updateAuxiliaryText ();
        return;
    }

    m_buffer.clear ();

    if (m_selected_special_phrase.empty ()) {
        size_t si = 0;
        size_t m_text_len = m_text.length();
        for (size_t i = m_phrase_editor.cursor (); i < m_pinyin.size (); ++i) {
            if (G_LIKELY (i != m_phrase_editor.cursor ()))
                m_buffer << ',';
            m_buffer << (unichar *)m_pinyin[i]->bopomofo;
            for (size_t sj = 0; m_pinyin[i]->bopomofo[sj] == bopomofo_char[keyvalToBopomofo(m_text.c_str()[si])] ; si++,sj++);

            if (si < m_text_len) {
                int ch = keyvalToBopomofo(m_text.c_str()[si]);
                if (ch >= BOPOMOFO_TONE_2 && ch <= BOPOMOFO_TONE_5) {
                    m_buffer.appendUnichar(bopomofo_char[ch]);
                    ++si;
                }
            }
        }

        for (String::iterator i = m_text.begin () + m_pinyin_len; i != m_text.end (); i++) {
            if (m_cursor == (size_t)(i - m_text.begin ()))
                m_buffer << '|';
            m_buffer.appendUnichar (bopomofo_char[keyvalToBopomofo (*i)]);
        }
        if (m_cursor == m_text.length ())
            m_buffer << '|';
    }
    else {
        if (m_cursor < m_text.size ()) {
            m_buffer << '|' << textAfterCursor ();
        }
    }

    m_auxiliary_text = m_buffer;
    PhoneticContext::updateAuxiliaryText ();
}

void
BopomofoContext::commit (CommitType type)
{
    if (G_UNLIKELY (m_buffer.empty ()))
        return;

    m_buffer.clear ();

    if (G_LIKELY (type == TYPE_CONVERTED)) {
        m_buffer << m_phrase_editor.selectedString ();

        const char *p;

        if (m_selected_special_phrase.empty ()) {
            p = textAfterPinyin (m_buffer.utf8Length ());
        }
        else {
            m_buffer << m_selected_special_phrase;
            p = textAfterCursor ();
        }

        while (*p != '\0') {
            m_buffer.appendUnichar ((unichar)bopomofo_char[keyvalToBopomofo (*p++)]);
        }

        m_phrase_editor.commit ();
    }
    else if (type == TYPE_PHONETIC) {
        const char *p = m_text;
        while (*p != '\0') {
            m_buffer.appendUnichar ((unichar)bopomofo_char[keyvalToBopomofo (*p++)]);
        }
    } else {
        m_buffer = m_text;
        m_phrase_editor.reset ();
    }

    resetContext ();
    updateInputText ();
    updateCursor ();
    update ();
    PhoneticContext::commitText (m_buffer);
}

void
BopomofoContext::updatePreeditText (void)
{
    /* preedit text = selected phrases + highlight candidate + rest text */
    if (G_UNLIKELY (m_phrase_editor.empty () && m_text.empty ())) {
        m_preedit_text.clear ();
        PhoneticContext::updatePreeditText ();
        return;
    }

    size_t edit_begin_byte = 0;
    size_t edit_end_byte = 0;

    m_buffer.clear ();
    m_preedit_text.clear ();

    /* add selected phrases */
    m_buffer << m_phrase_editor.selectedString ();

    if (G_UNLIKELY (! m_selected_special_phrase.empty ())) {
        /* add selected special phrase */
        m_buffer << m_selected_special_phrase;
        edit_begin_byte = edit_end_byte = m_buffer.size ();

        /* append text after cursor */
        m_buffer << textAfterCursor ();
    }
    else {
        edit_begin_byte = m_buffer.size ();

        if (hasCandidate (0)) {
            size_t index = m_focused_candidate;

            if (index < m_special_phrases.size ()) {
                m_buffer << m_special_phrases[index].c_str ();
                edit_end_byte = m_buffer.size ();

                /* append text after cursor */
                m_buffer << textAfterCursor ();
            }
            else {
                const Phrase & candidate = m_phrase_editor.candidate (index - m_special_phrases.size ());
                if (m_text.size () == m_cursor) {
                    /* cursor at end */
                    if (m_config.modeSimp)
                        m_buffer << candidate;
                    else
                        SimpTradConverter::simpToTrad (candidate, m_buffer);
                    edit_end_byte = m_buffer.size ();
                    /* append rest text */
                    for (const char *p=m_text.c_str() + m_pinyin_len; *p ;++p) {
                        m_buffer.appendUnichar(bopomofo_char[keyvalToBopomofo(*p)]);
                    }
                }
                else {
                    for (const char *p = m_text.c_str (); *p; ++p) {
                        if ((size_t) (p - m_text.c_str ()) == m_cursor)
                            m_buffer << ' ';
                        m_buffer.appendUnichar (bopomofo_char[keyvalToBopomofo (*p)]);
                    }
                    edit_end_byte = m_buffer.size ();
                }
            }
        }
        else {
            edit_end_byte = m_buffer.size ();
            for (const char *p=m_text.c_str () + m_pinyin_len; *p ; ++p) {
                m_buffer.appendUnichar (bopomofo_char[keyvalToBopomofo (*p)]);
            }
        }
    }

    m_preedit_text.selected_text = m_buffer.substr (0, edit_begin_byte);
    m_preedit_text.candidate_text = m_buffer.substr (edit_begin_byte, edit_end_byte - edit_begin_byte);
    m_preedit_text.rest_text = m_buffer.substr (edit_end_byte);

    PhoneticContext::updatePreeditText ();
}

Variant
BopomofoContext::getProperty (PropertyName name) const
{
    if (name == PROPERTY_BOPOMOFO_SCHEMA) {
        return Variant::fromUnsignedInt(m_bopomofo_schema);
    }
    return PhoneticContext::getProperty (name);
}

bool
BopomofoContext::setProperty (PropertyName name, const Variant &variant)
{
    if (name == PROPERTY_BOPOMOFO_SCHEMA) {
        if (variant.getType () != Variant::TYPE_UNSIGNED_INT) {
            return false;
        }
        const unsigned int schema = variant.getUnsignedInt ();
        if (schema >= BOPOMOFO_KEYBOARD_LAST) {
            return false;
        }

        m_bopomofo_schema = schema;
        return true;
    }

    return PhoneticContext::setProperty (name, variant);
}

static int
keyboard_cmp (const void * p1, const void * p2)
{
    const int s1 = GPOINTER_TO_INT (p1);
    const unsigned char *s2 = (const unsigned char *) p2;
    return s1 - s2[0];
}

int
BopomofoContext::keyvalToBopomofo(int ch)
{
    const unsigned int keyboard = m_bopomofo_schema;
    const unsigned char *brs;
    brs = (const unsigned char *) std::bsearch (GINT_TO_POINTER (ch),
                                       bopomofo_keyboard[keyboard],
                                       G_N_ELEMENTS (bopomofo_keyboard[keyboard]),
                                       sizeof(bopomofo_keyboard[keyboard][0]),
                                       keyboard_cmp);
    if (G_UNLIKELY (brs == NULL))
        return BOPOMOFO_ZERO;
    return brs[1];
}

};  // namespace PyZy
