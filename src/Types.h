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
#ifndef __PYZY_TYPES_H_
#define __PYZY_TYPES_H_

#include <cstddef>

namespace PyZy {

#define PINYIN_ID_VOID  (-1)
#define PINYIN_ID_ZERO  (0)
#define PINYIN_ID_B     (1)
#define PINYIN_ID_C     (2)
#define PINYIN_ID_CH    (3)
#define PINYIN_ID_D     (4)
#define PINYIN_ID_F     (5)
#define PINYIN_ID_G     (6)
#define PINYIN_ID_H     (7)
#define PINYIN_ID_J     (8)
#define PINYIN_ID_K     (9)
#define PINYIN_ID_L     (10)
#define PINYIN_ID_M     (11)
#define PINYIN_ID_N     (12)
#define PINYIN_ID_P     (13)
#define PINYIN_ID_Q     (14)
#define PINYIN_ID_R     (15)
#define PINYIN_ID_S     (16)
#define PINYIN_ID_SH    (17)
#define PINYIN_ID_T     (18)
#define PINYIN_ID_W     (19)
#define PINYIN_ID_X     (20)
#define PINYIN_ID_Y     (21)
#define PINYIN_ID_Z     (22)
#define PINYIN_ID_ZH    (23)
#define PINYIN_ID_A     (24)
#define PINYIN_ID_AI    (25)
#define PINYIN_ID_AN    (26)
#define PINYIN_ID_ANG   (27)
#define PINYIN_ID_AO    (28)
#define PINYIN_ID_E     (29)
#define PINYIN_ID_EI    (30)
#define PINYIN_ID_EN    (31)
#define PINYIN_ID_ENG   (32)
#define PINYIN_ID_ER    (33)
#define PINYIN_ID_I     (34)
#define PINYIN_ID_IA    (35)
#define PINYIN_ID_IAN   (36)
#define PINYIN_ID_IANG  (37)
#define PINYIN_ID_IAO   (38)
#define PINYIN_ID_IE    (39)
#define PINYIN_ID_IN    (40)
#define PINYIN_ID_ING   (41)
#define PINYIN_ID_IONG  (42)
#define PINYIN_ID_IU    (43)
#define PINYIN_ID_O     (44)
#define PINYIN_ID_ONG   (45)
#define PINYIN_ID_OU    (46)
#define PINYIN_ID_U     (47)
#define PINYIN_ID_UA    (48)
#define PINYIN_ID_UAI   (49)
#define PINYIN_ID_UAN   (50)
#define PINYIN_ID_UANG  (51)
#define PINYIN_ID_UE    (52)
#define PINYIN_ID_VE    PINYIN_ID_UE
#define PINYIN_ID_UI    (53)
#define PINYIN_ID_UN    (54)
#define PINYIN_ID_UO    (55)
#define PINYIN_ID_V     (56)
#define PINYIN_ID_NG    PINYIN_ID_VOID

struct Pinyin {
    const char *text;
    const wchar_t *bopomofo;
    const char *sheng;
    const char *yun;
    struct {
        unsigned char sheng;
        unsigned char yun;
    } pinyin_id[3];
    const size_t len;
    const unsigned int flags;
};

#define MAX_UTF8_LEN 6
#define MAX_PHRASE_LEN 16

};  // namespace PyZy

#endif  // __PYZY_TYPES_H_
