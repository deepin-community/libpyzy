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
#ifndef __PYZY_UTIL_H_
#define __PYZY_UTIL_H_

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#if defined(HAVE_UUID_CREATE)
#  include <uuid.h>
#elif defined(HAVE_LIBUUID)
#  include <uuid/uuid.h>
#endif

#include <sys/utsname.h>
#include <cstdlib>
#include <string>

#ifdef __GXX_EXPERIMENTAL_CXX0X__
#  include <memory>
#else
#  include <boost/shared_ptr.hpp>
#  include <boost/scoped_ptr.hpp>

namespace std {
    // import boost::shared_ptr to std namespace
    using boost::shared_ptr;
    // import boost::scoped_ptr to std namespace, and rename to unique_ptr
    // XXX: the unique_ptr can transfer the pointer ownership,
    //      but scoped_ptr cannot.
    template<typename T> class unique_ptr : public boost::scoped_ptr<T> {};
};
#endif  // __GXX_EXPERIMENTAL_CXX0X__PYZY_

namespace PyZy {
// for Unicode
typedef unsigned int unichar;

class UUID {
public:
    UUID (void)
    {
        uuid_t u;
#if defined(HAVE_UUID_CREATE)
        char *uuid;
        uuid_create (&u, 0);
        uuid_to_string (&u, &uuid, 0);
        g_strlcpy (m_uuid, uuid, sizeof(m_uuid));
        free(uuid);
#elif defined(HAVE_LIBUUID)
        uuid_generate (u);
        uuid_unparse_lower (u, m_uuid);
#endif
    }

    operator const char * (void) const
    {
        return m_uuid;
    }

private:
    char m_uuid[256];
};

class Uname {
public:
    Uname (void)
    {
        uname (&m_buf);
    }

    const char *hostname (void) const { return m_buf.nodename; }
private:
    struct utsname m_buf;
};

class Hostname : public Uname {
public:
    operator const char * (void) const
    {
        return hostname ();
    }
};

class Env : public std::string {
public:
    Env (const char *name)
    {
        char *str;
        str = std::getenv (name);
        assign (str != NULL ? str : "");
    }

    operator const char *(void) const
    {
        return c_str();
    }
};

};  // namespace PyZy

#endif  // __PYZY_UTIL_H_
