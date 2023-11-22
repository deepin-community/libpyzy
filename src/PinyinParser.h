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
#ifndef __PYZY_PINYIN_PARSER_H_
#define __PYZY_PINYIN_PARSER_H_

#include "PinyinArray.h"
#include "String.h"

namespace PyZy {

class PinyinParser {
public:
    static size_t parse (const String &pinyin,      // pinyin string
                         size_t        len,         // length of pinyin string
                         unsigned int  option,      // option
                         PinyinArray  &result,      // store pinyin in result
                         size_t        max);        // max length of the result
    static const Pinyin * isPinyin (int sheng, int yun, unsigned int option);
    static size_t parseBopomofo (const std::wstring  &bopomofo,
                                 size_t               len,
                                 unsigned int         option,
                                 PinyinArray         &result,
                                 size_t               max);
    static bool isBopomofoToneChar (const wchar_t ch);

};

};  // namespace PyZy

#endif  // __PYZY_PINYIN_PARSER_H_
