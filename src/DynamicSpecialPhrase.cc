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
#include "DynamicSpecialPhrase.h"

#include <glib.h>

namespace PyZy {

DynamicSpecialPhrase::~DynamicSpecialPhrase (void)
{
}

std::string
DynamicSpecialPhrase::text (void)
{
    /* get the current time */
    std::time_t rawtime;
    std::time (&rawtime);
    m_time = *std::localtime (&rawtime);

    std::string result;

    size_t pos = 0;
    size_t pnext;
    int s = 0;
    while (s != 2) {
        switch (s) {
        case 0: // expect "${"
            pnext = m_text.find ("${", pos);
            if (pnext == m_text.npos) {
                result += m_text.substr (pos);
                s = 2;
            }
            else {
                result += m_text.substr (pos, pnext - pos);
                pos = pnext + 2;
                s = 1;
            }
            break;
        case 1: // expect "}"
            pnext = m_text.find ("}", pos);
            if (pnext == m_text.npos) {
                result += "${";
                result += m_text.substr (pos);
                s = 2;
            }
            else {
                result += variable (m_text.substr(pos, pnext - pos));
                pos = pnext + 1;
                s = 0;
            }
            break;
        default: /* should not be reached */
            g_assert_not_reached ();
        }
    }
    return result;
}

inline const std::string
DynamicSpecialPhrase::dec (int d, const char *fmt)
{
    char string [32];
    g_snprintf (string, sizeof (string), fmt, d);
    return string;
}

inline const std::string
DynamicSpecialPhrase::year_cn (bool yy)
{
    static const char * const digits[] = {
        "〇", "一", "二", "三", "四",
        "五", "六", "七", "八", "九"
    };

    int year = m_time.tm_year + 1900;
    int bit = 0;
    if (yy) {
        year %= 100;
        bit = 2;
    }

    std::string result;
    while (year != 0 || bit > 0) {
        result.insert(0, digits[year % 10]);
        year /= 10;
        bit -= 1;
    }
    return result;
}

inline const std::string
DynamicSpecialPhrase::month_cn (void)
{
    static const char * const month_num[] = {
        "一", "二", "三", "四", "五", "六", "七", "八",
        "九", "十", "十一", "十二"
    };
    return month_num[m_time.tm_mon];
}

inline const std::string
DynamicSpecialPhrase::weekday_cn (void)
{
    static const char * const week_num[] = {
        "日", "一", "二", "三", "四", "五", "六"
    };
    return week_num[m_time.tm_wday];
}

inline const std::string
DynamicSpecialPhrase::hour_cn (unsigned int i)
{
    static const char * const hour_num[] = {
        "零", "一", "二", "三", "四",
        "五", "六", "七", "八", "九",
        "十", "十一", "十二", "十三", "十四",
        "十五", "十六", "十七", "十八", "十九",
        "二十", "二十一", "二十二", "二十三",
    };
    return hour_num[i];
}

inline const std::string
DynamicSpecialPhrase::fullhour_cn (void)
{
    return hour_cn (m_time.tm_hour);
}

inline const std::string
DynamicSpecialPhrase::halfhour_cn (void)
{
    return hour_cn (m_time.tm_hour % 12);
}

inline const std::string
DynamicSpecialPhrase::day_cn (void)
{
    static const char * const day_num[] = {
        "", "一", "二", "三", "四",
        "五", "六", "七", "八", "九",
        "", "十","二十", "三十"
    };
    unsigned int day = m_time.tm_mday;
    return std::string (day_num[day / 10 + 10]) + day_num[day % 10];
}

inline const std::string
DynamicSpecialPhrase::minsec_cn (unsigned int i)
{
    static const char * const num[] = {
        "", "一", "二", "三", "四",
        "五", "六", "七", "八", "九",
        "零", "十","二十", "三十", "四十"
        "五十", "六十"
    };
    return std::string (num[i / 10 + 10]) + num[i % 10];
}

inline const std::string
DynamicSpecialPhrase::variable (const std::string &name)
{
    if (name == "year")     return dec (m_time.tm_year + 1900);
    if (name == "year_yy")  return dec ((m_time.tm_year + 1900) % 100, "%02d");
    if (name == "month")    return dec (m_time.tm_mon + 1);
    if (name == "month_mm") return dec (m_time.tm_mon + 1, "%02d");
    if (name == "day")      return dec (m_time.tm_mday);
    if (name == "day_dd")   return dec (m_time.tm_mday, "%02d");
    if (name == "weekday")  return dec (m_time.tm_wday + 1);
    if (name == "fullhour") return dec (m_time.tm_hour, "%02d");
    if (name == "falfhour") return dec (m_time.tm_hour % 12, "%02d");
    if (name == "ampm")     return m_time.tm_hour < 12 ? "AM" : "PM";
    if (name == "minute")   return dec (m_time.tm_min, "%02d");
    if (name == "second")   return dec (m_time.tm_sec, "%02d");
    if (name == "year_cn")      return year_cn ();
    if (name == "year_yy_cn")   return year_cn (true);
    if (name == "month_cn")     return month_cn ();
    if (name == "day_cn")       return day_cn ();
    if (name == "weekday_cn")   return weekday_cn ();
    if (name == "fullhour_cn")  return fullhour_cn ();
    if (name == "halfhour_cn")  return halfhour_cn ();
    if (name == "ampm_cn")      return m_time.tm_hour < 12 ? "上午" : "下午";
    if (name == "minute_cn")    return minsec_cn (m_time.tm_min);
    if (name == "second_cn")    return minsec_cn (m_time.tm_sec);

    return "${" + name + "}";
}

};  // namespace PyZy
