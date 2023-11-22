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
#ifndef __PYZY_PINYIN_ARRAY_H_
#define __PYZY_PINYIN_ARRAY_H_

#include <vector>

#include "Types.h"

namespace PyZy {

struct PinyinSegment {
    const Pinyin *pinyin;
    size_t begin;
    size_t len;

    PinyinSegment (const Pinyin *pinyin = NULL, size_t begin = 0, size_t len = 0)
        : pinyin (pinyin), begin (begin), len (len) { }

    operator const Pinyin * (void) const
    {
        return pinyin;
    }

    const Pinyin * operator-> (void) const
    {
        return pinyin;
    }

    bool operator == (const PinyinSegment & p) const
    {
        return (pinyin == p.pinyin) && (begin == p.begin) && (len == p.len);
    }

    bool operator == (const Pinyin *p) const
    {
        return pinyin == p;
    }
};

class PinyinArray: public std::vector<PinyinSegment> {
public:
    PinyinArray (size_t init_size = 0)
    {
        std::vector<PinyinSegment>::reserve (init_size);
    }

    void append (const Pinyin *pinyin, size_t begin, size_t len)
    {
        push_back (PinyinSegment (pinyin, begin, len));
    }
};

};  // namespace PyZy

#endif  // __PYZY_PINYIN_ARRAY_H_
