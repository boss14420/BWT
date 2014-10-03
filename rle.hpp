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
    assert(min_run > 1);
    if (n == 0) return 0;
    if (n == 1) { dst[0] = src[0]; return 1; }

    typedef typename std::iterator_traits<pcstring_type>::value_type char_type;
    static_assert(std::is_unsigned<char_type>::value, "source string require unsigned char_type");
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
            while (run >= max_run) {
                std::fill_n(dst, min_run, c1);
                dst += min_run;
                *dst++ = std::min(run, max_run);
                run -= std::min(run, max_run);
            }
            if (run > 0) {
                std::fill_n(dst, run, c1);
                dst += run;
                if (run >= min_run)
                    *dst++ = run;
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
std::pair<std::size_t, std::size_t>
rle_encode2(pstring_type chr, pstring_type run_length, pcstring_type src, std::size_t n, std::size_t min_run=2)
{
    assert(min_run > 1);
    if (n == 0) return {0, 0};
    if (n == 1) { chr[0] = src[0]; return {1, 0}; }

    typedef typename std::iterator_traits<pcstring_type>::value_type char_type;
    static_assert(std::is_unsigned<char_type>::value, "source string require unsigned char_type");
    std::size_t const max_run = std::numeric_limits<char_type>::max();
    auto chr_begin = chr, rl_begin = run_length;

    auto s_end = src + n;
    char_type c0 = *src++;

    bool end_at_run = false;
    while(src != s_end) {
        char_type c1 = *src++;
        if (c0 == c1) {
            // run
            std::size_t run = 2;
            while ( (src != s_end) && ((c0 = *src++) == c1) ) ++run;
            while (run >= max_run) {
                std::fill_n(chr, min_run, c1);
                chr += min_run;
                //*dst++ = std::min(run, max_run);
                *run_length++ = std::min(run, max_run);
                run -= std::min(run, max_run);
            }
            if (run > 0) {
                std::fill_n(chr, std::min(run, min_run), c1);
                chr += std::min(run, min_run);
                if (run >= min_run)
                    *run_length++ = run;
            }
            end_at_run = (src == s_end) && (c0 == c1);
        } else {
            *chr++ = c0;
            c0 = c1;
        }
    }
    if (!end_at_run) *chr++ = c0;
    return { chr - chr_begin, run_length - rl_begin };
}

template<typename pstring_type, typename pcstring_type>
std::pair<std::size_t, std::size_t>
rle_encode3(pstring_type chr, pstring_type run_length, pcstring_type src, std::size_t n)
{
    if (n == 0) return {0, 0};
    if (n == 1) { chr[0] = src[0]; run_length[0] = 1; return {1, 1}; }

    typedef typename std::iterator_traits<pcstring_type>::value_type char_type;
    static_assert(std::is_unsigned<char_type>::value, "source string require unsigned char_type");
    std::size_t const max_run = std::numeric_limits<char_type>::max();
    auto chr_begin = chr, rl_begin = run_length;

    auto s_end = src + n;
    char_type c0, c1;

    while(src != s_end) {
        std::size_t run = 1;
        c0 = *src++;
        while (src != s_end && ((c1 = *src++) == c0)) ++run;
        if (c1 != c0) --src;
        while (run >= max_run) {
            *chr++ = c0;
            *run_length++ = max_run;
            run -= max_run;
        }
        if (run > 0) {
            *chr++ = c0;
            *run_length++ = run;
        }
    }
    return { chr - chr_begin, run_length - rl_begin };
}


template<typename pstring_type, typename pcstring_type>
std::size_t
rle_decode(pstring_type dst, pcstring_type src, std::size_t n, std::size_t min_run=2)
{
    assert(min_run > 1);
    if (n == 0) return 0;
    if (n == 1) { dst[0] = src[0]; return 1; }

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

template<typename pstring_type, typename pcstring_type>
std::size_t
rle_decode2(pstring_type dst, pcstring_type chr, std::size_t chr_len,
            pcstring_type run_length, std::size_t rl_len, std::size_t min_run=2)
{
    assert(min_run > 1);
    if (chr_len == 0) return 0;
    if (chr_len == 1) { dst[0] = chr [0]; return 1; }

    typedef typename std::iterator_traits<pcstring_type>::value_type char_type;
    static_assert(std::is_unsigned<char_type>::value, "source string require unsigned char_type");
    auto dst_begin = dst;

    auto s_end = chr + chr_len;
    char_type c0 = *chr++;

    bool has_next = true;
    while(chr != s_end) {
        char_type c1 = *chr++;
        if (c0 == c1) {
            // run
            std::size_t run = 2;
            while ( (chr != s_end) && ((c0 = *chr++) == c1)) ++run;
            while (run >= min_run) {
                std::fill_n(dst, *run_length, c1);
                dst += *run_length++;
                run -= min_run;
            }
            if (run > 0) {
                std::fill_n(dst, run, c1);
                dst += run;
            }
            if (chr == s_end && c0 == c1) has_next = false;
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
