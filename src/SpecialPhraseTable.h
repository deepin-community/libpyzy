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
#ifndef __PYZY_SPECIAL_PHRASE_TABLE_H_
#define __PYZY_SPECIAL_PHRASE_TABLE_H_

#include <glib.h>
#include <map>
#include <string>
#include <vector>

#include "Util.h"

namespace PyZy {

class SpecialPhrase;
typedef std::shared_ptr<SpecialPhrase> SpecialPhrasePtr;

class SpecialPhraseTable {
private:
    explicit SpecialPhraseTable (const std::string &config_dir);

public:
    bool lookup (const std::string &command, std::vector<std::string> &result);

private:
    bool load (const char *file);

public:
    static void init (const std::string &config_dir);
    static SpecialPhraseTable & instance (void);

private:
    typedef std::multimap<std::string, SpecialPhrasePtr> Map;
    Map m_map;

private:
    static std::unique_ptr<SpecialPhraseTable> m_instance;
};

};  // namespace PyZy

#endif  // __PYZY_SPECIAL_PHRASE_TABLE_H_
