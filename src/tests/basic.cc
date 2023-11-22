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
#include <glib/gstdio.h>

#include <iostream>
#include <algorithm>

#include "Config.h"
#include "InputContext.h"
#include "Util.h"  // for unique_ptr
#include "Variant.h"


using namespace std;
using namespace PyZy;

class DummyObserver : public PyZy::InputContext::Observer {
public:
    void commitText (const InputContext *context, const std::string &commit_text) {
        m_commited_text = commit_text;
    }
    void inputTextChanged (const InputContext *context) {}
    void preeditTextChanged (const InputContext *context) {}
    void auxiliaryTextChanged (const InputContext *context) {}
    void candidatesChanged (const InputContext *context) {}
    void cursorChanged (const InputContext *context) {}

    string commitedText ()         { return m_commited_text; }

    void clear () {
        m_commited_text.clear ();
    }
private:
    string           m_commited_text;
};

void insertKeys (InputContext *context, const string &keys) {
    for (size_t i = 0; i < keys.size (); ++i) {
        context->insert (keys[i]);
    }
}

#define g_assert_cmpstring(s1, cmp, s2) \
    g_assert_cmpstr (s1.c_str(), cmp, s2)

int testFullPinyin ()
{
    DummyObserver observer;
    unique_ptr<InputContext> context;
    context.reset (InputContext::create (InputContext::FULL_PINYIN, &observer));

    {  // Reset
        context->reset ();

        observer.clear ();
        insertKeys (context.get (), "nihao");
        g_assert_cmpint (context->cursor (), ==, 5);
        g_assert_cmpstring (context->inputText (),  ==, "nihao");
        g_assert_cmpstring (context->selectedText (), ==, "");
        g_assert_cmpstring (context->conversionText (), ==, "你好");
        g_assert_cmpstring (context->restText (), ==, "");
        g_assert_cmpstring (context->auxiliaryText (), ==, "ni hao|");
        g_assert (context->hasCandidate (0));
        g_assert_cmpstring (observer.commitedText (), ==, "");

        observer.clear ();
        context->reset ();
        g_assert_cmpint (context->cursor (), ==, 0);
        g_assert_cmpstring (context->inputText (), ==, "");
        g_assert_cmpstring (context->selectedText (), ==, "");
        g_assert_cmpstring (context->conversionText (), ==, "");
        g_assert_cmpstring (context->restText (), ==, "");
        g_assert_cmpstring (context->auxiliaryText (), ==, "");
        g_assert (!context->hasCandidate (0));
        g_assert_cmpstring (observer.commitedText (), ==, "");
    }

    {  // Commit directly
        context->reset ();

        observer.clear ();
        insertKeys (context.get (), "nihao");
        g_assert_cmpint (context->cursor (), ==, 5);
        g_assert_cmpstring (context->inputText (), ==, "nihao");
        g_assert_cmpstring (context->selectedText (), ==, "");
        g_assert_cmpstring (context->conversionText (), ==, "你好");
        g_assert_cmpstring (context->restText (), ==, "");
        g_assert_cmpstring (context->auxiliaryText (), ==, "ni hao|");
        g_assert (context->hasCandidate (0));
        g_assert_cmpstring (observer.commitedText (), ==, "");

        observer.clear ();
        context->commit (InputContext::TYPE_CONVERTED);
        g_assert_cmpint (context->cursor (), ==, 0);
        g_assert_cmpstring (context->inputText (), ==, "");
        g_assert_cmpstring (context->selectedText (), ==, "");
        g_assert_cmpstring (context->conversionText (), ==, "");
        g_assert_cmpstring (context->restText (), ==, "");
        g_assert_cmpstring (context->auxiliaryText (), ==, "");
        g_assert (!context->hasCandidate (0));
        g_assert_cmpstring (observer.commitedText (), ==, "nihao");
    }

    {  // Select candidate
        context->reset ();

        observer.clear ();
        insertKeys (context.get (), "nihao");
        g_assert_cmpint (context->cursor (), ==, 5);
        g_assert_cmpstring (context->inputText (), ==, "nihao");
        g_assert_cmpstring (context->selectedText (), ==, "");
        g_assert_cmpstring (context->conversionText (), ==, "你好");
        g_assert_cmpstring (context->restText (), ==, "");
        g_assert_cmpstring (context->auxiliaryText (), ==, "ni hao|");
        g_assert (context->hasCandidate (0));
        g_assert_cmpstring (observer.commitedText (), ==, "");

        observer.clear ();
        context->selectCandidate (0);
        g_assert_cmpint (context->cursor (), ==, 0);
        g_assert_cmpstring (context->inputText (), ==, "");
        g_assert_cmpstring (context->selectedText (), ==, "");
        g_assert_cmpstring (context->conversionText (), ==, "");
        g_assert_cmpstring (context->restText (), ==, "");
        g_assert_cmpstring (context->auxiliaryText (), ==, "");
        g_assert (!context->hasCandidate (0));
        g_assert_cmpstring (observer.commitedText (), ==, "你好");
    }

    {  // Select special phrases.
        context->reset ();

        observer.clear ();
        insertKeys (context.get (), "aazhi");
        g_assert_cmpint (context->cursor (), ==, 5);
        g_assert_cmpstring (context->inputText (), ==, "aazhi");
        g_assert_cmpstring (context->selectedText (), ==, "");
        g_assert_cmpstring (context->conversionText (), ==, "AA制");
        g_assert_cmpstring (context->restText (), ==, "");
        g_assert_cmpstring (context->auxiliaryText (), ==, "aazhi|");
        g_assert (context->hasCandidate (0));
        g_assert_cmpstring (observer.commitedText (), ==, "");

        observer.clear ();
        context->selectCandidate (0);
        g_assert_cmpint (context->cursor (), ==, 0);
        g_assert_cmpstring (context->inputText (), ==, "");
        g_assert_cmpstring (context->selectedText (), ==, "");
        g_assert_cmpstring (context->conversionText (), ==, "");
        g_assert_cmpstring (context->restText (), ==, "");
        g_assert_cmpstring (context->auxiliaryText (), ==, "");
        g_assert (!context->hasCandidate (0));
        g_assert_cmpstring (observer.commitedText (), ==, "AA制");
    }

    {  // Select special phrases with some operations.
        context->reset ();

        observer.clear ();
        insertKeys (context.get (), "aazhii");
        g_assert_cmpint (context->cursor (), ==, 6);
        g_assert_cmpstring (context->inputText (), ==, "aazhii");
        g_assert_cmpstring (context->selectedText (), ==, "");
        g_assert_cmpstring (context->conversionText (), ==, "啊啊之");
        g_assert_cmpstring (context->restText (), ==, "i");
        g_assert_cmpstring (context->auxiliaryText (), ==, "a a zhi i|");
        g_assert (context->hasCandidate (0));
        g_assert_cmpstring (observer.commitedText (), ==, "");

        observer.clear ();
        context->moveCursorLeft ();
        g_assert_cmpint (context->cursor (), ==, 5);
        g_assert_cmpstring (context->inputText (), ==, "aazhii");
        g_assert_cmpstring (context->selectedText (), ==, "");
        g_assert_cmpstring (context->conversionText (), ==, "AA制");
        g_assert_cmpstring (context->restText (), ==, "i");
        g_assert_cmpstring (context->auxiliaryText (), ==, "aazhi|i");
        g_assert (context->hasCandidate (0));
        g_assert_cmpstring (observer.commitedText (), ==, "");

        observer.clear ();
        context->selectCandidate (0);
        g_assert_cmpint (context->cursor (), ==, 5);
        g_assert_cmpstring (context->inputText (), ==, "aazhii");
        g_assert_cmpstring (context->selectedText (), ==, "AA制");
        g_assert_cmpstring (context->conversionText (), ==, "");
        g_assert_cmpstring (context->restText (), ==, "i");
        g_assert_cmpstring (context->auxiliaryText (), ==, "");
        g_assert (!context->hasCandidate (0));
        g_assert_cmpstring (observer.commitedText (), ==, "");

        observer.clear ();
        context->commit (InputContext::TYPE_CONVERTED);
        g_assert_cmpint (context->cursor (), ==, 0);
        g_assert_cmpstring (context->inputText (), ==, "");
        g_assert_cmpstring (context->selectedText (), ==, "");
        g_assert_cmpstring (context->conversionText (), ==, "");
        g_assert_cmpstring (context->restText (), ==, "");
        g_assert_cmpstring (context->auxiliaryText (), ==, "");
        g_assert (!context->hasCandidate (0));
        g_assert_cmpstring (observer.commitedText (), ==, "AA制i");
    }

    { // Many operations
        context->reset ();

        observer.clear ();
        insertKeys (context.get (), "aazhi");
        g_assert_cmpint (context->cursor (), ==, 5);
        g_assert_cmpstring (context->inputText (), ==, "aazhi");
        g_assert_cmpstring (context->selectedText (), ==, "");
        g_assert_cmpstring (context->conversionText (), ==, "AA制");
        g_assert_cmpstring (context->restText (), ==, "");
        g_assert_cmpstring (context->auxiliaryText (), ==, "aazhi|");
        g_assert (context->hasCandidate (0));
        g_assert_cmpstring (observer.commitedText (), ==, "");

        observer.clear ();
        context->focusCandidate (1);
        g_assert_cmpint (context->cursor (), ==, 5);
        g_assert_cmpstring (context->inputText (), ==, "aazhi");
        g_assert_cmpstring (context->selectedText (), ==, "");
        g_assert_cmpstring (context->conversionText (), ==, "啊啊之");
        g_assert_cmpstring (context->restText (), ==, "");
        g_assert_cmpstring (context->auxiliaryText (), ==, "aazhi|");
        g_assert (context->hasCandidate (0));
        g_assert_cmpstring (observer.commitedText (), ==, "");

        observer.clear ();
        context->selectCandidate (4);
        g_assert_cmpint (context->cursor (), ==, 5);
        g_assert_cmpstring (context->inputText (), ==, "aazhi");
        g_assert_cmpstring (context->selectedText (), ==, "阿");
        g_assert_cmpstring (context->conversionText (), ==, "\xE9\x98\xBF\xE7\xB4\xAB");
        g_assert_cmpstring (context->restText (), ==, "");
        g_assert_cmpstring (context->auxiliaryText (), ==, "a zhi|");
        g_assert (context->hasCandidate (0));
        g_assert_cmpstring (observer.commitedText (), ==, "");

        observer.clear ();
        context->unselectCandidates ();
        g_assert_cmpint (context->cursor (), ==, 5);
        g_assert_cmpstring (context->inputText (), ==, "aazhi");
        g_assert_cmpstring (context->selectedText (), ==, "");
        g_assert_cmpstring (context->conversionText (), ==, "AA制");
        g_assert_cmpstring (context->restText (), ==, "");
        g_assert_cmpstring (context->auxiliaryText (), ==, "aazhi|");
        g_assert (context->hasCandidate (0));
        g_assert_cmpstring (observer.commitedText (), ==, "");

        observer.clear ();
        context->moveCursorLeft ();
        g_assert_cmpint (context->cursor (), ==, 4);
        g_assert_cmpstring (context->inputText (), ==, "aazhi");
        g_assert_cmpstring (context->selectedText (), ==, "");
        g_assert_cmpstring (context->conversionText (), ==, "a a zh|i");
        g_assert_cmpstring (context->restText (), ==, "");
        g_assert_cmpstring (context->auxiliaryText (), ==, "a a zh|i");
        g_assert (context->hasCandidate (0));
        g_assert_cmpstring (observer.commitedText (), ==, "");

        observer.clear ();
        context->insert ('i');
        g_assert_cmpint (context->cursor (), ==, 5);
        g_assert_cmpstring (context->inputText (), ==, "aazhii");
        g_assert_cmpstring (context->selectedText (), ==, "");
        g_assert_cmpstring (context->conversionText (), ==, "AA制");
        g_assert_cmpstring (context->restText (), ==, "i");
        g_assert_cmpstring (context->auxiliaryText (), ==, "aazhi|i");
        g_assert (context->hasCandidate (0));
        g_assert_cmpstring (observer.commitedText (), ==, "");

        observer.clear ();
        context->removeCharBefore ();
        g_assert_cmpint (context->cursor (), ==, 4);
        g_assert_cmpstring (context->inputText (), ==, "aazhi");
        g_assert_cmpstring (context->selectedText (), ==, "");
        g_assert_cmpstring (context->conversionText (), ==, "a a zh|i");
        g_assert_cmpstring (context->restText (), ==, "");
        g_assert_cmpstring (context->auxiliaryText (), ==, "a a zh|i");
        g_assert (context->hasCandidate (0));
        g_assert_cmpstring (observer.commitedText (), ==, "");

        observer.clear ();
        context->moveCursorRight ();
        g_assert_cmpint (context->cursor (), ==, 5);
        g_assert_cmpstring (context->inputText (), ==, "aazhi");
        g_assert_cmpstring (context->selectedText (), ==, "");
        g_assert_cmpstring (context->conversionText (), ==, "AA制");
        g_assert_cmpstring (context->restText (), ==, "");
        g_assert_cmpstring (context->auxiliaryText (), ==, "aazhi|");
        g_assert (context->hasCandidate (0));
        g_assert_cmpstring (observer.commitedText (), ==, "");

        observer.clear ();
        context->removeWordBefore ();
        g_assert_cmpint (context->cursor (), ==, 2);
        g_assert_cmpstring (context->inputText (), ==, "aa");
        g_assert_cmpstring (context->selectedText (), ==, "");
        g_assert_cmpstring (context->conversionText (), ==, "啊啊");
        g_assert_cmpstring (context->restText (), ==, "");
        g_assert_cmpstring (context->auxiliaryText (), ==, "a a|");
        g_assert (context->hasCandidate (0));
        g_assert_cmpstring (observer.commitedText (), ==, "");

        observer.clear ();
        insertKeys (context.get (), "nihao");
        g_assert_cmpint (context->cursor (), ==, 7);
        g_assert_cmpstring (context->inputText (), ==, "aanihao");
        g_assert_cmpstring (context->selectedText (), ==, "");
        g_assert_cmpstring (context->conversionText (), ==, "啊啊你好");
        g_assert_cmpstring (context->restText (), ==, "");
        g_assert_cmpstring (context->auxiliaryText (), ==, "a a ni hao|");
        g_assert (context->hasCandidate (0));
        g_assert_cmpstring (observer.commitedText (), ==, "");

        observer.clear ();
        context->selectCandidate (1);
        g_assert_cmpint (context->cursor (), ==, 7);
        g_assert_cmpstring (context->inputText (), ==, "aanihao");
        g_assert_cmpstring (context->selectedText (), ==, "啊啊");
        g_assert_cmpstring (context->conversionText (), ==, "你好");
        g_assert_cmpstring (context->restText (), ==, "");
        g_assert_cmpstring (context->auxiliaryText (), ==, "ni hao|");
        g_assert (context->hasCandidate (0));
        g_assert_cmpstring (observer.commitedText (), ==, "");

        observer.clear ();
        context->commit (InputContext::TYPE_CONVERTED);
        g_assert_cmpint (context->cursor (), ==, 0);
        g_assert_cmpstring (context->inputText (), ==, "");
        g_assert_cmpstring (context->selectedText (), ==, "");
        g_assert_cmpstring (context->conversionText (), ==, "");
        g_assert_cmpstring (context->restText (), ==, "");
        g_assert_cmpstring (context->auxiliaryText (), ==, "");
        g_assert (!context->hasCandidate (0));
        g_assert_cmpstring (observer.commitedText (), ==, "啊啊nihao");
    }
}

void testDoublePinyin()
{
    DummyObserver observer;
    unique_ptr<InputContext> context;
    context.reset (
        InputContext::create (InputContext::DOUBLE_PINYIN, &observer));

    {  // Reset
        context->reset ();

        observer.clear ();
        insertKeys (context.get (), "nihk");
        g_assert_cmpint (context->cursor (), ==, 4);
        g_assert_cmpstring (context->inputText (), ==, "nihk");
        g_assert_cmpstring (context->selectedText (), ==, "");
        g_assert_cmpstring (context->conversionText (), ==, "你好");
        g_assert_cmpstring (context->restText (), ==, "");
        g_assert_cmpstring (context->auxiliaryText (), ==, "ni hao|");
        g_assert (context->hasCandidate (0));
        g_assert_cmpstring (observer.commitedText (), ==, "");

        observer.clear ();
        context->reset ();
        g_assert_cmpint (context->cursor (), ==, 0);
        g_assert_cmpstring (context->inputText (), ==, "");
        g_assert_cmpstring (context->selectedText (), ==, "");
        g_assert_cmpstring (context->conversionText (), ==, "");
        g_assert_cmpstring (context->restText (), ==, "");
        g_assert_cmpstring (context->auxiliaryText (), ==, "");
        g_assert (!context->hasCandidate (0));
        g_assert_cmpstring (observer.commitedText (), ==, "");
    }

    {  // Commit directly
        context->reset ();

        observer.clear ();
        insertKeys (context.get (), "nihk");
        g_assert_cmpint (context->cursor (), ==, 4);
        g_assert_cmpstring (context->inputText (), ==, "nihk");
        g_assert_cmpstring (context->selectedText (), ==, "");
        g_assert_cmpstring (context->conversionText (), ==, "你好");
        g_assert_cmpstring (context->restText (), ==, "");
        g_assert_cmpstring (context->auxiliaryText (), ==, "ni hao|");
        g_assert (context->hasCandidate (0));
        g_assert_cmpstring (observer.commitedText (), ==, "");

        observer.clear ();
        context->commit (InputContext::TYPE_CONVERTED);
        g_assert_cmpint (context->cursor (), ==, 0);
        g_assert_cmpstring (context->inputText (), ==, "");
        g_assert_cmpstring (context->selectedText (), ==, "");
        g_assert_cmpstring (context->conversionText (), ==, "");
        g_assert_cmpstring (context->restText (), ==, "");
        g_assert_cmpstring (context->auxiliaryText (), ==, "");
        g_assert (!context->hasCandidate (0));
        g_assert_cmpstring (observer.commitedText (), ==, "nihk");
    }

    {  // Select candidate
        context->reset ();

        observer.clear ();
        insertKeys (context.get (), "nihk");
        g_assert_cmpint (context->cursor (), ==, 4);
        g_assert_cmpstring (context->inputText (), ==, "nihk");
        g_assert_cmpstring (context->selectedText (), ==, "");
        g_assert_cmpstring (context->conversionText (), ==, "你好");
        g_assert_cmpstring (context->restText (), ==, "");
        g_assert_cmpstring (context->auxiliaryText (), ==, "ni hao|");
        g_assert (context->hasCandidate (0));
        g_assert_cmpstring (observer.commitedText (), ==, "");

        observer.clear ();
        context->selectCandidate (0);
        g_assert_cmpint (context->cursor (), ==, 0);
        g_assert_cmpstring (context->inputText (), ==, "");
        g_assert_cmpstring (context->selectedText (), ==, "");
        g_assert_cmpstring (context->conversionText (), ==, "");
        g_assert_cmpstring (context->restText (), ==, "");
        g_assert_cmpstring (context->auxiliaryText (), ==, "");
        g_assert (!context->hasCandidate (0));
        g_assert_cmpstring (observer.commitedText (), ==, "你好");
    }

    {  // Select special phrases.
        context->reset ();

        observer.clear ();
        insertKeys (context.get (), "aazhi");
        g_assert_cmpint (context->cursor (), ==, 5);
        g_assert_cmpstring (context->inputText (), ==, "aazhi");
        g_assert_cmpstring (context->selectedText (), ==, "");
        g_assert_cmpstring (context->conversionText (), ==, "AA制");
        g_assert_cmpstring (context->restText (), ==, "");
        g_assert_cmpstring (context->auxiliaryText (), ==, "aazhi|");
        g_assert (context->hasCandidate (0));
        g_assert_cmpstring (observer.commitedText (), ==, "");

        observer.clear ();
        context->selectCandidate (0);
        g_assert_cmpint (context->cursor (), ==, 0);
        g_assert_cmpstring (context->inputText (), ==, "");
        g_assert_cmpstring (context->selectedText (), ==, "");
        g_assert_cmpstring (context->conversionText (), ==, "");
        g_assert_cmpstring (context->restText (), ==, "");
        g_assert_cmpstring (context->auxiliaryText (), ==, "");
        g_assert (!context->hasCandidate (0));
        g_assert_cmpstring (observer.commitedText (), ==, "AA制");
    }

    {  // Select special phrases with some operations.
        context->reset ();

        observer.clear ();
        insertKeys (context.get (), "aazhii");
        g_assert_cmpint (context->cursor (), ==, 6);
        g_assert_cmpstring (context->inputText (), ==, "aazhii");
        g_assert_cmpstring (context->selectedText (), ==, "");
        g_assert_cmpstring (context->conversionText (), ==, "啊展翅");
        g_assert_cmpstring (context->restText (), ==, "");
        g_assert_cmpstring (context->auxiliaryText (), ==, "a zang chi|");
        g_assert (context->hasCandidate (0));
        g_assert_cmpstring (observer.commitedText (), ==, "");

        observer.clear ();
        context->moveCursorLeft ();
        g_assert_cmpint (context->cursor (), ==, 5);
        g_assert_cmpstring (context->inputText (), ==, "aazhii");
        g_assert_cmpstring (context->selectedText (), ==, "");
        g_assert_cmpstring (context->conversionText (), ==, "AA制");
        g_assert_cmpstring (context->restText (), ==, "i");
        g_assert_cmpstring (context->auxiliaryText (), ==, "aazhi|i");
        g_assert (context->hasCandidate (0));
        g_assert_cmpstring (observer.commitedText (), ==, "");

        observer.clear ();
        context->selectCandidate (0);
        g_assert_cmpint (context->cursor (), ==, 5);
        g_assert_cmpstring (context->inputText (), ==, "aazhii");
        g_assert_cmpstring (context->selectedText (), ==, "AA制");
        g_assert_cmpstring (context->conversionText (), ==, "");
        g_assert_cmpstring (context->restText (), ==, "i");
        g_assert_cmpstring (context->auxiliaryText (), ==, "");
        g_assert (!context->hasCandidate (0));
        g_assert_cmpstring (observer.commitedText (), ==, "");

        observer.clear ();
        context->commit (InputContext::TYPE_CONVERTED);
        g_assert_cmpint (context->cursor (), ==, 0);
        g_assert_cmpstring (context->inputText (), ==, "");
        g_assert_cmpstring (context->selectedText (), ==, "");
        g_assert_cmpstring (context->conversionText (), ==, "");
        g_assert_cmpstring (context->restText (), ==, "");
        g_assert_cmpstring (context->auxiliaryText (), ==, "");
        g_assert (!context->hasCandidate (0));
        g_assert_cmpstring (observer.commitedText (), ==, "AA制i");
    }

    { // Many operations
        context->reset ();

        observer.clear ();
        insertKeys (context.get (), "aazhi");
        g_assert_cmpint (context->cursor (), ==, 5);
        g_assert_cmpstring (context->inputText (), ==, "aazhi");
        g_assert_cmpstring (context->selectedText (), ==, "");
        g_assert_cmpstring (context->conversionText (), ==, "AA制");
        g_assert_cmpstring (context->restText (), ==, "");
        g_assert_cmpstring (context->auxiliaryText (), ==, "aazhi|");
        g_assert (context->hasCandidate (0));
        g_assert_cmpstring (observer.commitedText (), ==, "");

        observer.clear ();
        context->focusCandidate (1);
        g_assert_cmpint (context->cursor (), ==, 5);
        g_assert_cmpstring (context->inputText (), ==, "aazhi");
        g_assert_cmpstring (context->selectedText (), ==, "");
        g_assert_cmpstring (context->conversionText (), ==, "啊战场");
        g_assert_cmpstring (context->restText (), ==, "");
        g_assert_cmpstring (context->auxiliaryText (), ==, "aazhi|");
        g_assert (context->hasCandidate (0));
        g_assert_cmpstring (observer.commitedText (), ==, "");

        observer.clear ();
        context->selectCandidate (3);
        g_assert_cmpint (context->cursor (), ==, 5);
        g_assert_cmpstring (context->inputText (), ==, "aazhi");
        g_assert_cmpstring (context->selectedText (), ==, "阿");
        g_assert_cmpstring (context->conversionText (), ==, "战场");
        g_assert_cmpstring (context->restText (), ==, "");
        g_assert_cmpstring (context->auxiliaryText (), ==, "zang ch|");
        g_assert (context->hasCandidate (0));
        g_assert_cmpstring (observer.commitedText (), ==, "");

        observer.clear ();
        context->unselectCandidates ();
        g_assert_cmpint (context->cursor (), ==, 5);
        g_assert_cmpstring (context->inputText (), ==, "aazhi");
        g_assert_cmpstring (context->selectedText (), ==, "");
        g_assert_cmpstring (context->conversionText (), ==, "AA制");
        g_assert_cmpstring (context->restText (), ==, "");
        g_assert_cmpstring (context->auxiliaryText (), ==, "aazhi|");
        g_assert (context->hasCandidate (0));
        g_assert_cmpstring (observer.commitedText (), ==, "");

        observer.clear ();
        context->moveCursorLeft ();
        g_assert_cmpint (context->cursor (), ==, 4);
        g_assert_cmpstring (context->inputText (), ==, "aazhi");
        g_assert_cmpstring (context->selectedText (), ==, "");
        g_assert_cmpstring (context->conversionText (), ==, "a zang|i");
        g_assert_cmpstring (context->restText (), ==, "");
        g_assert_cmpstring (context->auxiliaryText (), ==, "a zang|i");
        g_assert (context->hasCandidate (0));
        g_assert_cmpstring (observer.commitedText (), ==, "");

        observer.clear ();
        context->insert ('i');
        g_assert_cmpint (context->cursor (), ==, 5);
        g_assert_cmpstring (context->inputText (), ==, "aazhii");
        g_assert_cmpstring (context->selectedText (), ==, "");
        g_assert_cmpstring (context->conversionText (), ==, "AA制");
        g_assert_cmpstring (context->restText (), ==, "i");
        g_assert_cmpstring (context->auxiliaryText (), ==, "aazhi|i");
        g_assert (context->hasCandidate (0));
        g_assert_cmpstring (observer.commitedText (), ==, "");

        observer.clear ();
        context->removeCharBefore ();
        g_assert_cmpint (context->cursor (), ==, 4);
        g_assert_cmpstring (context->inputText (), ==, "aazhi");
        g_assert_cmpstring (context->selectedText (), ==, "");
        g_assert_cmpstring (context->conversionText (), ==, "a zang|i");
        g_assert_cmpstring (context->restText (), ==, "");
        g_assert_cmpstring (context->auxiliaryText (), ==, "a zang|i");
        g_assert (context->hasCandidate (0));
        g_assert_cmpstring (observer.commitedText (), ==, "");

        observer.clear ();
        context->moveCursorRight ();
        g_assert_cmpint (context->cursor (), ==, 5);
        g_assert_cmpstring (context->inputText (), ==, "aazhi");
        g_assert_cmpstring (context->selectedText (), ==, "");
        g_assert_cmpstring (context->conversionText (), ==, "AA制");
        g_assert_cmpstring (context->restText (), ==, "");
        g_assert_cmpstring (context->auxiliaryText (), ==, "aazhi|");
        g_assert (context->hasCandidate (0));
        g_assert_cmpstring (observer.commitedText (), ==, "");

        observer.clear ();
        context->removeWordBefore ();
        g_assert_cmpint (context->cursor (), ==, 4);
        g_assert_cmpstring (context->inputText (), ==, "aazh");
        g_assert_cmpstring (context->selectedText (), ==, "");
        g_assert_cmpstring (context->conversionText (), ==, "啊张");
        g_assert_cmpstring (context->restText (), ==, "");
        g_assert_cmpstring (context->auxiliaryText (), ==, "a zang|");
        g_assert (context->hasCandidate (0));
        g_assert_cmpstring (observer.commitedText (), ==, "");

        observer.clear ();
        insertKeys (context.get (), "nihk");
        g_assert_cmpint (context->cursor (), ==, 8);
        g_assert_cmpstring (context->inputText (), ==, "aazhnihk");
        g_assert_cmpstring (context->selectedText (), ==, "");
        g_assert_cmpstring (context->conversionText (), ==, "啊站立好");
        g_assert_cmpstring (context->restText (), ==, "");
        g_assert_cmpstring (context->auxiliaryText (), ==, "a zang ni hao|");
        g_assert (context->hasCandidate (0));
        g_assert_cmpstring (observer.commitedText (), ==, "");

        observer.clear ();
        context->selectCandidate (1);
        g_assert_cmpint (context->cursor (), ==, 8);
        g_assert_cmpstring (context->inputText (), ==, "aazhnihk");
        g_assert_cmpstring (context->selectedText (), ==, "啊");
        g_assert_cmpstring (context->conversionText (), ==, "站立好");
        g_assert_cmpstring (context->restText (), ==, "");
        g_assert_cmpstring (context->auxiliaryText (), ==, "zang ni hao|");
        g_assert (context->hasCandidate (0));
        g_assert_cmpstring (observer.commitedText (), ==, "");

        observer.clear ();
        context->commit (InputContext::TYPE_CONVERTED);
        g_assert_cmpint (context->cursor (), ==, 0);
        g_assert_cmpstring (context->inputText (), ==, "");
        g_assert_cmpstring (context->selectedText (), ==, "");
        g_assert_cmpstring (context->conversionText (), ==, "");
        g_assert_cmpstring (context->restText (), ==, "");
        g_assert_cmpstring (context->auxiliaryText (), ==, "");
        g_assert (!context->hasCandidate (0));
        g_assert_cmpstring (observer.commitedText (), ==, "啊zhnihk");
    }
}

void testBopomofo ()
{
    DummyObserver observer;
    unique_ptr<InputContext> context;
    context.reset (InputContext::create (InputContext::BOPOMOFO, &observer));
    context->setProperty (InputContext::PROPERTY_SPECIAL_PHRASE,
                          Variant::fromBool (false));

    {  // Reset
        context->reset ();

        observer.clear ();
        insertKeys (context.get (), "sucl");
        g_assert_cmpint (context->cursor (), ==, 4);
        g_assert_cmpstring (context->inputText (), ==, "sucl");
        g_assert_cmpstring (context->selectedText (), ==, "");
        g_assert_cmpstring (context->conversionText (), ==, "你好");
        g_assert_cmpstring (context->restText (), ==, "");
        g_assert_cmpstring (context->auxiliaryText (), ==, "ㄋㄧ,ㄏㄠ|");
        g_assert (context->hasCandidate (0));
        g_assert_cmpstring (observer.commitedText (), ==, "");

        observer.clear ();
        context->reset ();
        g_assert_cmpint (context->cursor (), ==, 0);
        g_assert_cmpstring (context->inputText (), ==, "");
        g_assert_cmpstring (context->selectedText (), ==, "");
        g_assert_cmpstring (context->conversionText (), ==, "");
        g_assert_cmpstring (context->restText (), ==, "");
        g_assert_cmpstring (context->auxiliaryText (), ==, "");
        g_assert (!context->hasCandidate (0));
        g_assert_cmpstring (observer.commitedText (), ==, "");
    }

    {  // Commit directly
        context->reset ();

        observer.clear ();
        insertKeys (context.get (), "sucl");
        g_assert_cmpint (context->cursor (), ==, 4);
        g_assert_cmpstring (context->inputText (), ==, "sucl");
        g_assert_cmpstring (context->selectedText (), ==, "");
        g_assert_cmpstring (context->conversionText (), ==, "你好");
        g_assert_cmpstring (context->restText (), ==, "");
        g_assert_cmpstring (context->auxiliaryText (), ==, "ㄋㄧ,ㄏㄠ|");
        g_assert (context->hasCandidate (0));
        g_assert_cmpstring (observer.commitedText (), ==, "");

        observer.clear ();
        context->commit (InputContext::TYPE_RAW);
        g_assert_cmpint (context->cursor (), ==, 0);
        g_assert_cmpstring (context->inputText (), ==, "");
        g_assert_cmpstring (context->selectedText (), ==, "");
        g_assert_cmpstring (context->conversionText (), ==, "");
        g_assert_cmpstring (context->restText (), ==, "");
        g_assert_cmpstring (context->auxiliaryText (), ==, "");
        g_assert (!context->hasCandidate (0));
        g_assert_cmpstring (observer.commitedText (), ==, "sucl");
    }

    {  // Select candidate
        context->reset ();

        observer.clear ();
        insertKeys (context.get (), "sucl");
        g_assert_cmpint (context->cursor (), ==, 4);
        g_assert_cmpstring (context->inputText (), ==, "sucl");
        g_assert_cmpstring (context->selectedText (), ==, "");
        g_assert_cmpstring (context->conversionText (), ==, "你好");
        g_assert_cmpstring (context->restText (), ==, "");
        g_assert_cmpstring (context->auxiliaryText (), ==, "ㄋㄧ,ㄏㄠ|");
        g_assert (context->hasCandidate (0));
        g_assert_cmpstring (observer.commitedText (), ==, "");

        observer.clear ();
        context->selectCandidate (0);
        g_assert_cmpint (context->cursor (), ==, 0);
        g_assert_cmpstring (context->inputText (), ==, "");
        g_assert_cmpstring (context->selectedText (), ==, "");
        g_assert_cmpstring (context->conversionText (), ==, "");
        g_assert_cmpstring (context->restText (), ==, "");
        g_assert_cmpstring (context->auxiliaryText (), ==, "");
        g_assert (!context->hasCandidate (0));
        g_assert_cmpstring (observer.commitedText (), ==, "你好");
    }

    {  // Don't use special phrases for bopomofo.
        context->reset ();

        observer.clear ();
        insertKeys (context.get (), "aazhi");
        g_assert_cmpint (context->cursor (), ==, 5);
        g_assert_cmpstring (context->inputText (), ==, "aazhi");
        g_assert_cmpstring (context->selectedText (), ==, "");
        g_assert_cmpstring (context->conversionText (), ==, "妈妈好吃哦");
        g_assert_cmpstring (context->restText (), ==, "");
        g_assert_cmpstring (context->auxiliaryText (), ==, "ㄇ,ㄇ,ㄈ,ㄘ,ㄛ|");
        g_assert (context->hasCandidate (0));
        g_assert_cmpstring (observer.commitedText (), ==, "");

        observer.clear ();
        context->selectCandidate (0);
        g_assert_cmpint (context->cursor (), ==, 0);
        g_assert_cmpstring (context->inputText (), ==, "");
        g_assert_cmpstring (context->selectedText (), ==, "");
        g_assert_cmpstring (context->conversionText (), ==, "");
        g_assert_cmpstring (context->restText (), ==, "");
        g_assert_cmpstring (context->auxiliaryText (), ==, "");
        g_assert (!context->hasCandidate (0));
        g_assert_cmpstring (observer.commitedText (), ==, "妈妈好吃哦");
    }

    {  // Select special phrases with some operations.
        context->reset ();

        observer.clear ();
        insertKeys (context.get (), "aazhii");
        g_assert_cmpint (context->cursor (), ==, 6);
        g_assert_cmpstring (context->inputText (), ==, "aazhii");
        g_assert_cmpstring (context->selectedText (), ==, "");
        g_assert_cmpstring (context->conversionText (), ==, "妈妈好吃哦哦");
        g_assert_cmpstring (context->restText (), ==, "");
        g_assert_cmpstring (context->auxiliaryText (), ==, "ㄇ,ㄇ,ㄈ,ㄘ,ㄛ,ㄛ|");
        g_assert (context->hasCandidate (0));
        g_assert_cmpstring (observer.commitedText (), ==, "");

        observer.clear ();
        context->moveCursorLeft ();
        g_assert_cmpint (context->cursor (), ==, 5);
        g_assert_cmpstring (context->inputText (), ==, "aazhii");
        g_assert_cmpstring (context->selectedText (), ==, "");
        g_assert_cmpstring (context->conversionText (), ==, "ㄇㄇㄈㄘㄛ ㄛ");
        g_assert_cmpstring (context->restText (), ==, "");
        g_assert_cmpstring (context->auxiliaryText (), ==, "ㄇ,ㄇ,ㄈ,ㄘ,ㄛ|ㄛ");
        g_assert (context->hasCandidate (0));
        g_assert_cmpstring (observer.commitedText (), ==, "");

        observer.clear ();
        context->selectCandidate (0);
        g_assert_cmpint (context->cursor (), ==, 5);
        g_assert_cmpstring (context->inputText (), ==, "aazhii");
        g_assert_cmpstring (context->selectedText (), ==, "妈妈好吃哦");
        g_assert_cmpstring (context->conversionText (), ==, "");
        g_assert_cmpstring (context->restText (), ==, "ㄛ");
        g_assert_cmpstring (context->auxiliaryText (), ==, "");
        g_assert (!context->hasCandidate (0));
        g_assert_cmpstring (observer.commitedText (), ==, "");

        observer.clear ();
        context->commit (InputContext::TYPE_CONVERTED);
        g_assert_cmpint (context->cursor (), ==, 0);
        g_assert_cmpstring (context->inputText (), ==, "");
        g_assert_cmpstring (context->selectedText (), ==, "");
        g_assert_cmpstring (context->conversionText (), ==, "");
        g_assert_cmpstring (context->restText (), ==, "");
        g_assert_cmpstring (context->auxiliaryText (), ==, "");
        g_assert (!context->hasCandidate (0));
        g_assert_cmpstring (observer.commitedText (), ==, "妈妈好吃哦ㄛ");
    }

    { // Many operations
        context->reset ();

        observer.clear ();
        insertKeys (context.get (), "aazhi");
        g_assert_cmpint (context->cursor (), ==, 5);
        g_assert_cmpstring (context->inputText (), ==, "aazhi");
        g_assert_cmpstring (context->selectedText (), ==, "");
        g_assert_cmpstring (context->conversionText (), ==, "妈妈好吃哦");
        g_assert_cmpstring (context->restText (), ==, "");
        g_assert_cmpstring (context->auxiliaryText (), ==, "ㄇ,ㄇ,ㄈ,ㄘ,ㄛ|");
        g_assert (context->hasCandidate (0));
        g_assert_cmpstring (observer.commitedText (), ==, "");

        observer.clear ();
        context->focusCandidate (1);
        g_assert_cmpint (context->cursor (), ==, 5);
        g_assert_cmpstring (context->inputText (), ==, "aazhi");
        g_assert_cmpstring (context->selectedText (), ==, "");
        g_assert_cmpstring (context->conversionText (), ==, "妈妈");
        g_assert_cmpstring (context->restText (), ==, "");
        g_assert_cmpstring (context->auxiliaryText (), ==, "ㄇ,ㄇ,ㄈ,ㄘ,ㄛ|");
        g_assert (context->hasCandidate (0));
        g_assert_cmpstring (observer.commitedText (), ==, "");

        observer.clear ();
        context->selectCandidate (3);
        g_assert_cmpint (context->cursor (), ==, 5);
        g_assert_cmpstring (context->inputText (), ==, "aazhi");
        g_assert_cmpstring (context->selectedText (), ==, "慢慢");
        g_assert_cmpstring (context->conversionText (), ==, "好吃哦");
        g_assert_cmpstring (context->restText (), ==, "");
        g_assert_cmpstring (context->auxiliaryText (), ==, "ㄈ,ㄘ,ㄛ|");
        g_assert (context->hasCandidate (0));
        g_assert_cmpstring (observer.commitedText (), ==, "");

        observer.clear ();
        context->unselectCandidates ();
        g_assert_cmpint (context->cursor (), ==, 5);
        g_assert_cmpstring (context->inputText (), ==, "aazhi");
        g_assert_cmpstring (context->selectedText (), ==, "");
        g_assert_cmpstring (context->conversionText (), ==, "妈妈好吃哦");
        g_assert_cmpstring (context->restText (), ==, "");
        g_assert_cmpstring (context->auxiliaryText (), ==, "ㄇ,ㄇ,ㄈ,ㄘ,ㄛ|");
        g_assert (context->hasCandidate (0));
        g_assert_cmpstring (observer.commitedText (), ==, "");

        observer.clear ();
        context->moveCursorLeft ();
        g_assert_cmpint (context->cursor (), ==, 4);
        g_assert_cmpstring (context->inputText (), ==, "aazhi");
        g_assert_cmpstring (context->selectedText (), ==, "");
        g_assert_cmpstring (context->conversionText (), ==, "ㄇㄇㄈㄘ ㄛ");
        g_assert_cmpstring (context->restText (), ==, "");
        g_assert_cmpstring (context->auxiliaryText (), ==, "ㄇ,ㄇ,ㄈ,ㄘ|ㄛ");
        g_assert (context->hasCandidate (0));
        g_assert_cmpstring (observer.commitedText (), ==, "");

        observer.clear ();
        context->insert ('i');
        g_assert_cmpint (context->cursor (), ==, 5);
        g_assert_cmpstring (context->inputText (), ==, "aazhii");
        g_assert_cmpstring (context->selectedText (), ==, "");
        g_assert_cmpstring (context->conversionText (), ==, "ㄇㄇㄈㄘㄛ ㄛ");
        g_assert_cmpstring (context->restText (), ==, "");
        g_assert_cmpstring (context->auxiliaryText (), ==, "ㄇ,ㄇ,ㄈ,ㄘ,ㄛ|ㄛ");
        g_assert (context->hasCandidate (0));
        g_assert_cmpstring (observer.commitedText (), ==, "");

        observer.clear ();
        context->removeCharBefore ();
        g_assert_cmpint (context->cursor (), ==, 4);
        g_assert_cmpstring (context->inputText (), ==, "aazhi");
        g_assert_cmpstring (context->selectedText (), ==, "");
        g_assert_cmpstring (context->conversionText (), ==, "ㄇㄇㄈㄘ ㄛ");
        g_assert_cmpstring (context->restText (), ==, "");
        g_assert_cmpstring (context->auxiliaryText (), ==, "ㄇ,ㄇ,ㄈ,ㄘ|ㄛ");
        g_assert (context->hasCandidate (0));
        g_assert_cmpstring (observer.commitedText (), ==, "");

        observer.clear ();
        context->moveCursorRight ();
        g_assert_cmpint (context->cursor (), ==, 5);
        g_assert_cmpstring (context->inputText (), ==, "aazhi");
        g_assert_cmpstring (context->selectedText (), ==, "");
        g_assert_cmpstring (context->conversionText (), ==, "妈妈好吃哦");
        g_assert_cmpstring (context->restText (), ==, "");
        g_assert_cmpstring (context->auxiliaryText (), ==, "ㄇ,ㄇ,ㄈ,ㄘ,ㄛ|");
        g_assert (context->hasCandidate (0));
        g_assert_cmpstring (observer.commitedText (), ==, "");

        observer.clear ();
        context->removeWordBefore ();
        g_assert_cmpint (context->cursor (), ==, 4);
        g_assert_cmpstring (context->inputText (), ==, "aazh");
        g_assert_cmpstring (context->selectedText (), ==, "");
        g_assert_cmpstring (context->conversionText (), ==, "妈妈好吃");
        g_assert_cmpstring (context->restText (), ==, "");
        g_assert_cmpstring (context->auxiliaryText (), ==, "ㄇ,ㄇ,ㄈ,ㄘ|");
        g_assert (context->hasCandidate (0));
        g_assert_cmpstring (observer.commitedText (), ==, "");

        observer.clear ();
        insertKeys (context.get (), "nihk");
        g_assert_cmpint (context->cursor (), ==, 8);
        g_assert_cmpstring (context->inputText (), ==, "aazhnihk");
        g_assert_cmpstring (context->selectedText (), ==, "");
        g_assert_cmpstring (context->conversionText (), ==, "妈妈好吃是哦车");
        g_assert_cmpstring (context->restText (), ==, "");
        g_assert_cmpstring (context->auxiliaryText (), ==, "ㄇ,ㄇ,ㄈ,ㄘ,ㄙ,ㄛ,ㄘㄜ|");
        g_assert (context->hasCandidate (0));
        g_assert_cmpstring (observer.commitedText (), ==, "");

        observer.clear ();
        context->selectCandidate (1);
        g_assert_cmpint (context->cursor (), ==, 8);
        g_assert_cmpstring (context->inputText (), ==, "aazhnihk");
        g_assert_cmpstring (context->selectedText (), ==, "妈妈");
        g_assert_cmpstring (context->conversionText (), ==, "好吃是哦车");
        g_assert_cmpstring (context->restText (), ==, "");
        g_assert_cmpstring (context->auxiliaryText (), ==, "ㄈ,ㄘ,ㄙ,ㄛ,ㄘㄜ|");
        g_assert (context->hasCandidate (0));
        g_assert_cmpstring (observer.commitedText (), ==, "");

        observer.clear ();
        context->commit (InputContext::TYPE_CONVERTED);
        g_assert_cmpint (context->cursor (), ==, 0);
        g_assert_cmpstring (context->inputText (), ==, "");
        g_assert_cmpstring (context->selectedText (), ==, "");
        g_assert_cmpstring (context->conversionText (), ==, "");
        g_assert_cmpstring (context->restText (), ==, "");
        g_assert_cmpstring (context->auxiliaryText (), ==, "");
        g_assert (!context->hasCandidate (0));
        g_assert_cmpstring (observer.commitedText (), ==, "妈妈ㄈㄘㄙㄛㄘㄜ");
    }
}

void testCommit ()
{
    {  // Pinyin commit
        DummyObserver observer;
        unique_ptr<InputContext> context;
        context.reset (
            InputContext::create (InputContext::FULL_PINYIN, &observer));

        observer.clear ();
        insertKeys (context.get (), "nihao");
        g_assert_cmpstring (context->inputText (), ==, "nihao");
        g_assert_cmpstring (context->conversionText (), ==, "你好");
        g_assert_cmpstring (context->auxiliaryText (), ==, "ni hao|");

        observer.clear ();
        context->commit (InputContext::TYPE_RAW);
        g_assert_cmpstring (observer.commitedText (), ==, "nihao");

        context->reset ();
        observer.clear ();
        insertKeys (context.get (), "nihao");
        g_assert_cmpstring (context->inputText (), ==, "nihao");
        g_assert_cmpstring (context->conversionText (), ==, "你好");
        g_assert_cmpstring (context->auxiliaryText (), ==, "ni hao|");

        observer.clear ();
        context->commit (InputContext::TYPE_PHONETIC);
        g_assert_cmpstring (observer.commitedText (), ==, "nihao");

        context->reset ();
        observer.clear ();
        insertKeys (context.get (), "nihao");
        g_assert_cmpstring (context->inputText (), ==, "nihao");
        g_assert_cmpstring (context->conversionText (), ==, "你好");
        g_assert_cmpstring (context->auxiliaryText (), ==, "ni hao|");

        // To get "你好", we should call selectCandidate().
        observer.clear ();
        context->commit (InputContext::TYPE_CONVERTED);
        g_assert_cmpstring (observer.commitedText (), ==, "nihao");
    }

    {  // Bopomofo commit
        DummyObserver observer;
        unique_ptr<InputContext> context;
        context.reset (
            InputContext::create (InputContext::BOPOMOFO, &observer));

        observer.clear ();
        insertKeys (context.get (), "sucl");
        g_assert_cmpstring (context->inputText (), ==, "sucl");
        g_assert_cmpstring (context->conversionText (), ==, "你好");
        g_assert_cmpstring (context->auxiliaryText (), ==, "ㄋㄧ,ㄏㄠ|");

        observer.clear ();
        context->commit (InputContext::TYPE_RAW);
        g_assert_cmpstring (observer.commitedText (), ==, "sucl");

        context->reset ();
        observer.clear ();
        insertKeys (context.get (), "sucl");
        g_assert_cmpstring (context->inputText (), ==, "sucl");
        g_assert_cmpstring (context->conversionText (), ==, "你好");
        g_assert_cmpstring (context->auxiliaryText (), ==, "ㄋㄧ,ㄏㄠ|");

        observer.clear ();
        context->commit (InputContext::TYPE_PHONETIC);
        g_assert_cmpstring (observer.commitedText (), ==, "ㄋㄧㄏㄠ");

        context->reset ();
        observer.clear ();
        insertKeys (context.get (), "sucl");
        g_assert_cmpstring (context->inputText (), ==, "sucl");
        g_assert_cmpstring (context->conversionText (), ==, "你好");
        g_assert_cmpstring (context->auxiliaryText (), ==, "ㄋㄧ,ㄏㄠ|");

        // To get "你好", we should call selectCandidate().
        observer.clear ();
        context->commit (InputContext::TYPE_CONVERTED);
        g_assert_cmpstring (observer.commitedText (), ==, "ㄋㄧㄏㄠ");
    }
}

string getTestDir ()
{
    const char *kPyZyTestDirName = "__pyzy_test_dir__";

    gchar *path = g_build_filename (g_get_tmp_dir(), kPyZyTestDirName, NULL);
    const string result = path;
    g_free (path);
    return result;
}

bool removeDirectory (const string &path) {
    GDir *dir = g_dir_open (path.c_str (), 0, NULL);
    if (dir == NULL) {
        return false;
    }

    const gchar *entry_name = NULL;
    while ((entry_name = g_dir_read_name (dir)) != NULL) {
        gchar *entry_path = g_build_filename (path.c_str (), entry_name, NULL);

        if (g_file_test (entry_path, G_FILE_TEST_IS_DIR)) {
            removeDirectory (entry_path);
        } else {
            g_unlink (entry_path);
        }

        g_free (entry_path);
    }

    g_dir_close (dir);
    int ret = g_rmdir (path.c_str ());

    return ret == 0;
}

void setUp ()
{
    const string test_dir = getTestDir ();
    InputContext::init (test_dir, test_dir);
}

void tearDown ()
{
    InputContext::finalize ();
    removeDirectory (getTestDir ());
}

int main (int argc, char **argv)
{
    setUp();
    testFullPinyin();
    tearDown();

    setUp();
    testDoublePinyin();
    tearDown();

    setUp();
    testBopomofo();
    tearDown();

    setUp();
    testCommit();
    tearDown();

    return 0;
}
