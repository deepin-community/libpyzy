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
#include "Variant.h"

namespace PyZy {

Variant::Variant()
{
}

// static
Variant Variant::fromBool (bool value)
{
    Variant variant;
    variant.m_type = TYPE_BOOL;
    variant.m_value.bool_value = value;
    return variant;
}

// static
Variant Variant::fromUnsignedInt (unsigned int value)
{
    Variant variant;
    variant.m_type = TYPE_UNSIGNED_INT;
    variant.m_value.unsigned_int_value = value;
    return variant;
}

// static
Variant Variant::nullVariant (void)
{
    Variant variant;
    variant.m_type = TYPE_NULL;
    variant.m_value.unsigned_int_value = 0u;
    return variant;
}

Variant::Type Variant::getType (void) const
{
    return m_type;
}

bool Variant::getBool (void) const
{
    return m_value.bool_value;
}

unsigned int Variant::getUnsignedInt (void) const
{
    return m_value.unsigned_int_value;
}

}  // namespace PyZy
