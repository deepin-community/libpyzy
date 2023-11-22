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
#ifndef __PYZY_BOPOMOFO_CONTEXT_H_
#define __PYZY_BOPOMOFO_CONTEXT_H_

#include "PhoneticContext.h"

namespace PyZy {

class BopomofoContext : public PhoneticContext {

public:
    explicit BopomofoContext (PhoneticContext::Observer *observer);
    virtual ~BopomofoContext (void);

    /* API of InputContext */
    virtual bool insert (char ch);
    virtual void commit (CommitType type);

    virtual bool removeCharBefore (void);
    virtual bool removeCharAfter (void);
    virtual bool removeWordBefore (void);
    virtual bool removeWordAfter (void);

    virtual bool moveCursorLeft (void);
    virtual bool moveCursorRight (void);
    virtual bool moveCursorLeftByWord (void);
    virtual bool moveCursorRightByWord (void);
    virtual bool moveCursorToBegin (void);
    virtual bool moveCursorToEnd (void);

    virtual Variant getProperty (PropertyName name) const;
    virtual bool setProperty (PropertyName name, const Variant &variant);

protected:
    virtual void updateAuxiliaryText ();
    virtual void updatePinyin ();
    virtual void updatePreeditText ();

    bool processBopomofo (
        unsigned int keyval, unsigned int keycode, unsigned int modifiers);
    int keyvalToBopomofo(int ch);

private:
    unsigned int m_bopomofo_schema;
};

};

#endif  // __PYZY_BOPOMOFO_CONTEXT_H_
