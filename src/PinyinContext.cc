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
#include "PinyinContext.h"
#include "SimpTradConverter.h"

namespace PyZy {

PinyinContext::PinyinContext (PhoneticContext::Observer * observer)
    : PhoneticContext (observer)
{
}

PinyinContext::~PinyinContext ()
{
}

void
PinyinContext::commit (CommitType type)
{
    if (G_UNLIKELY (m_buffer.empty ()))
        return;

    m_buffer.clear ();

    if (G_LIKELY(type == TYPE_CONVERTED)) {
        m_buffer << m_phrase_editor.selectedString ();

        const char *p;

        if (m_selected_special_phrase.empty ()) {
            p = textAfterPinyin (m_buffer.utf8Length ());
        }
        else {
            m_buffer << m_selected_special_phrase;
            p = textAfterCursor ();
        }
        m_buffer << p;

        m_phrase_editor.commit ();
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
PinyinContext::updatePreeditText ()
{
    /* preedit text = selected phrases + highlight candidate + rest text */
    if (G_UNLIKELY (m_phrase_editor.empty () && m_text.empty ())) {
        m_preedit_text.clear ();
        PhoneticContext::updatePreeditText ();
        return;
    }

    size_t edit_begin_word = 0;
    size_t edit_end_word = 0;
    size_t edit_begin_byte = 0;
    size_t edit_end_byte = 0;

    m_buffer.clear ();
    m_preedit_text.clear ();

    /* add selected phrases */
    m_buffer << m_phrase_editor.selectedString ();

    if (G_UNLIKELY (! m_selected_special_phrase.empty ())) {
        /* add selected special phrase */
        m_buffer << m_selected_special_phrase;
        edit_begin_word = edit_end_word = m_buffer.utf8Length ();
        edit_begin_byte = edit_end_byte = m_buffer.size ();

        /* append text after cursor */
        m_buffer += textAfterCursor ();
    }
    else {
        edit_begin_word = m_buffer.utf8Length ();
        edit_begin_byte = m_buffer.size ();

        if (hasCandidate (0)) {
            size_t index = m_focused_candidate;
            if (index < m_special_phrases.size ()) {
                m_buffer << m_special_phrases[index];
                edit_end_word = m_buffer.utf8Length ();
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
                    edit_end_word = m_buffer.utf8Length ();
                    edit_end_byte = m_buffer.size ();

                    /* append rest text */
                    m_buffer << textAfterPinyin (edit_end_word);
                }
                else {
                    size_t candidate_end = edit_begin_word + candidate.len;

                    m_buffer << m_pinyin[edit_begin_word]->sheng << m_pinyin[edit_begin_word]->yun;

                    for (size_t i = edit_begin_word + 1; i < candidate_end; i++) {
                        m_buffer << ' ' << m_pinyin[i]->sheng << m_pinyin[i]->yun;
                    }
                    m_buffer << '|' << textAfterPinyin (candidate_end);
                    m_preedit_text.rest_text = m_buffer;

                    edit_end_word = m_buffer.utf8Length ();
                    edit_end_byte = m_buffer.size ();
                }
            }
        }
        else {
            edit_end_word = m_buffer.utf8Length ();
            edit_end_byte = m_buffer.size ();
            m_buffer << textAfterPinyin ();
        }
    }

    m_preedit_text.selected_text = m_buffer.substr (0, edit_begin_byte);
    m_preedit_text.candidate_text = m_buffer.substr (edit_begin_byte, edit_end_byte - edit_begin_byte);
    m_preedit_text.rest_text = m_buffer.substr (edit_end_byte);

    PhoneticContext::updatePreeditText ();
}

void
PinyinContext::updateAuxiliaryText (void)
{
    /* clear pinyin array */
    if (G_UNLIKELY (m_text.empty () || !hasCandidate (0))) {
        m_auxiliary_text = "";
        PhoneticContext::updateAuxiliaryText ();
        return;
    }

    m_buffer.clear ();

    if (m_selected_special_phrase.empty ()) {
        if (m_focused_candidate < m_special_phrases.size ()) {
            size_t begin = m_phrase_editor.cursorInChar ();
            m_buffer << m_text.substr (begin, m_cursor - begin)
                     << '|' << textAfterCursor ();
        }
        else {
            for (size_t i = m_phrase_editor.cursor (); i < m_pinyin.size (); ++i) {
                if (G_LIKELY (i != m_phrase_editor.cursor ()))
                    m_buffer << ' ';
                const Pinyin *p = m_pinyin[i];
                m_buffer << p->sheng
                         << p->yun;
            }

            if (G_UNLIKELY (m_pinyin_len == m_cursor)) {
                /* aux = pinyin + non-pinyin */
                // cursor_pos =  m_buffer.utf8Length ();
                m_buffer << '|' << textAfterPinyin ();
            }
            else {
                /* aux = pinyin + ' ' + non-pinyin before cursor + non-pinyin after cursor */
                m_buffer << ' ';
                m_buffer.append (textAfterPinyin (),
                             m_cursor - m_pinyin_len);
                // cursor_pos =  m_buffer.utf8Length ();
                m_buffer  << '|' << textAfterCursor ();
            }
        }
    }
    else {
        if (m_cursor < m_text.size ()) {
            m_buffer  << '|' << textAfterCursor ();
        }
    }

    m_auxiliary_text = m_buffer;
    PhoneticContext::updateAuxiliaryText ();
}

};  // namespace PyZy
