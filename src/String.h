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
#ifndef __PYZY_STRING_H_
#define __PYZY_STRING_H_

#include <glib.h>
#include <stdarg.h>
#include <string>

#include "Util.h"

namespace PyZy {

class String : public std::string {
public:
    String () : std::string () { }
    String (const char *str) : std::string (str) { }
    String (const std::string &str) : std::string (str) { }
    String (size_t len) : std::string () { reserve (len); }

    String & printf (const char *fmt, ...)
    {
        char *str;
        va_list args;

        va_start (args, fmt);
        str = g_strdup_vprintf (fmt, args);
        va_end (args);

        assign (str);
        g_free (str);
        return *this;
    }

    String & appendPrintf (const char *fmt, ...)
    {
        char *str;
        va_list args;

        va_start (args, fmt);
        str = g_strdup_vprintf (fmt, args);
        va_end (args);

        append (str);
        g_free (str);

        return *this;
    }

    String & appendUnichar (unichar ch)
    {
        char str[12];
        size_t len;
        len = g_unichar_to_utf8 (ch, str);
        str[len] = 0;
        append (str);
        return *this;
    }

    String & insert (size_t i, char ch)
    {
        std::string::insert (i, 1, ch);
        return *this;
    }

    String & truncate (size_t len)
    {
        erase(len);
        return *this;
    }

    String & replace (const char *pattern, const char *str)
    {
        String result;
        String::size_type pos = 0;
        String::size_type pos_before = 0;
        const String::size_type length = std::string (pattern).size ();

        while ((pos = this->find (pattern, pos)) != String::npos) {
            result.append (*this, pos_before, pos - pos_before);
            result.append (str);
            pos += length;
            pos_before = pos;
        }
        result.append (*this, pos_before, this->size () - pos_before);
        this->assign (result);
        return *this;
    }

    size_t utf8Length (void) const
    {
        return g_utf8_strlen (c_str(), -1);
    }

    String & operator<< (int i)
    {
        return appendPrintf ("%d", i);
    }

    String & operator<< (unsigned int i)
    {
        return appendPrintf ("%u", i);
    }

    String & operator<< (unsigned long i)
    {
        return appendPrintf ("%lu", i);
    }

    String & operator<< (const char ch)
    {
        append (1, ch);
        return *this;
    }

    String & operator<< (const char *str)
    {
        append (str);
        return *this;
    }

    String & operator<< (const unichar *wstr)
    {
        char *str;
        GError *error;
        str = g_ucs4_to_utf8 (wstr, -1, NULL, NULL, &error);
        if (str == NULL) {
            g_warning ("convert ucs4 to utf8 failed: %s", error->message);
            g_error_free (error);
        }
        else {
            append (str);
            g_free (str);
        }
        return *this;
    }

    char operator[] (size_t i)
    {
        return std::string::operator[] (i);
    }

    String & operator<< (const std::string &str)
    {
        return operator<< (str.c_str ());
    }

    String & operator<< (const String &str)
    {
        return operator<< ((const char *)str);
    }

    String & operator= (const char * str)
    {
        assign (str);
        return *this;
    }

    operator const char *(void) const
    {
        return this->c_str ();
    }

    operator bool (void) const
    {
        return ! empty ();
    }
};

};  // namespace PyZy

#endif  // __PYZY_STRING_H_
