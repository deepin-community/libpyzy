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
#ifndef __PYZY_VARIANT_H_
#define __PYZY_VARIANT_H_

namespace PyZy {

/**
 * \brief Variant class to handle various type of value.
 */
class Variant {
public:
    /**
     * \brief The type contained in the variant.
     */
    enum Type {
        /** null */
        TYPE_NULL,
        /** boolean value */
        TYPE_BOOL,
        /** unsigned int value */
        TYPE_UNSIGNED_INT,
    };

    /**
     * \brief Creates a Variant object from the argument.
     * @param value boolean value.
     * @return Variant object.
     */
    static Variant fromBool (bool value);

    /**
     * \brief Creates a Variant object from the argument.
     * @param value unsigned int value.
     * @return Variant object.
     */
    static Variant fromUnsignedInt (unsigned int value);

    /**
     * \brief Creates a null Variant object.
     * @return Variant object.
     */
    static Variant nullVariant (void);

    /**
     * \brief Gets a type of the contained value.
     * @return type of the contained value.
     * @see Type
     */
    Type getType (void) const;

    /**
     * \brief Gets a value as bool.
     * @return value of the variant as bool.
     *
     * This method doesn't check the type.
     */
    bool getBool (void) const;

    /**
     * \brief Gets a value as unsigned int.
     * @return value of the variant as bool.
     *
     * This method doesn't check the type.
     */
    unsigned int getUnsignedInt (void) const;

private:
    Variant();

    Type m_type;
    union {
        bool bool_value;
        unsigned int  unsigned_int_value;
    } m_value;
};

}  // namespace PyZy

#endif
