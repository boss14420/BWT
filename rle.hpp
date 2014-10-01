/*
 * =====================================================================================
 *
 *       Filename:  rle.hpp
 *
 *    Description:  run-length encoding
 *
 *        Version:  1.0
 *        Created:  10/01/2014 12:25:49 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  BOSS14420 (), firefoxlinux at gmail dot com
 *   Organization:
 *
 * =====================================================================================
 */

#ifndef __RLE_HPP__
#define __RLE_HPP__

#include <cstring>
#include <algorithm>
#include <cassert>

namespace rle {

template<typename pstring_type, typename pcstring_type>
std::size_t
rle_encode(pstring_type dst, pcstring_type src, std::size_t n, std::size_t min_run=2)
{
    if (n == 0) return 0;
    if (n == 1) { dst[0] = src[0]; return 1; }
    assert(min_run > 1);

    typedef typename std::iterator_traits<pcstring_type>::value_type char_type;
    std::size_t const max_run = std::numeric_limits<char_type>::max();
    auto dst_begin = dst;

    auto s_end = src + n;
    char_type c0 = *src++;

    bool end_at_run = false;
    while(src != s_end) {
        char_type c1 = *src++;
        if (c0 == c1) {
            // run
            std::size_t run = 2;
            while ( (src != s_end) && ((c0 = *src++) == c1) ) ++run;
            while (run > min_run-1) {
                std::fill_n(dst, min_run, c1);
                dst += min_run;
                *dst++ = std::min(run, max_run);
                run -= std::min(run, max_run);
            }
            if (run > 0) {
                std::fill_n(dst, run, c1);
                dst += run;
            }
            end_at_run = (src == s_end) && (c0 == c1);
        } else {
            *dst++ = c0;
            c0 = c1;
        }
    }
    if (!end_at_run) *dst++ = c0;
    return dst - dst_begin;
}


template<typename pstring_type, typename pcstring_type>
std::size_t
rle_decode(pstring_type dst, pcstring_type src, std::size_t n, std::size_t min_run=2)
{
    if (n == 0) return 0;
    if (n == 1) { dst[0] = src[0]; return 1; }
    assert(min_run > 1);

    typedef typename std::iterator_traits<pcstring_type>::value_type char_type;
    static_assert(std::is_unsigned<char_type>::value, "source string require unsigned char_type");
    auto dst_begin = dst;

    auto s_end = src + n;
    char_type c0 = *src++;

    bool has_next = true;
    while(src != s_end) {
        char_type c1 = *src++;
        if (c0 == c1) {
            // run
            std::size_t run = 2;
            while ( (src != s_end) && ((c0 = *src++) == c1) && run < min_run) ++run;
            std::fill_n(dst, run, c1); dst += run;
            if (run == min_run) {
                std::fill_n(dst, c0-run, c1);
                dst += c0-run;
                if (src != s_end) { c0 = *src++; }
                else has_next = false;
            } else if (c0 == c1) {
                if (src != s_end) { c0 = *src++;  }
                else has_next = false;
            }// else if (src == s_end) has_next = true;
        } else {
            *dst++ = c0;
            c0 = c1;
        }
    }
    if (has_next) *dst++ = c0;
    return dst - dst_begin;
}

} // namespace rle

#endif // __RLE_HPP__
