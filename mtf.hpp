/*
 * =====================================================================================
 *
 *       Filename:  mtf.hpp
 *
 *    Description:  Move-to-front coding
 *
 *        Version:  1.0
 *        Created:  09/30/2014 10:16:55 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  BOSS14420 (), firefoxlinux at gmail dot com
 *   Organization:
 *
 * =====================================================================================
 */

#ifndef __MTF_HPP__
#define __MTF_HPP__

#include <algorithm>
#include <vector>
#include <memory>
#include <chrono>
#include <cstring>
#include <numeric>

namespace mtf {

//! block size
static const std::size_t block_size = 200000;

/*
 * ===  FUNCTION  ======================================================================
 *         Name:  mtf_encode_block
 *  Description:
 *  \param dst: output string
 *  \param src: source string
 *  \param symbol: symbol table
 *  \param len: string len
 * =====================================================================================
 */
template <typename pcstring_type, typename pstring_type>
void mtf_encode_block(pstring_type dst, pcstring_type src, pstring_type symbol, std::size_t len)
{
    typedef typename std::iterator_traits<pstring_type>::value_type char_type;
    pcstring_type s_end = src + len;

    for (; src != s_end; ++src, ++dst) {
        char_type ch = *src, t0, t1, idx;

        idx = 0;
        if (symbol[0] != ch) {
            idx = 1;
            t1 = symbol[1];
            symbol[1] = symbol[0];
            pstring_type sptr = symbol + 1;
//            for (; t1 != ch; ++idx) {
//                t0 = symbol[idx+1];
//                symbol[idx+1] = t1;
//                std::swap(t0, t1);
//            }
            while (true) {
                if (t1 == ch) {            break; } t0 = sptr[ 1]; sptr[ 1] = t1;
                if (t0 == ch) { ++idx;     break; } t1 = sptr[ 2]; sptr[ 2] = t0;
                if (t1 == ch) { idx +=  2; break; } t0 = sptr[ 3]; sptr[ 3] = t1;
                if (t0 == ch) { idx +=  3; break; } t1 = sptr[ 4]; sptr[ 4] = t0;
                if (t1 == ch) { idx +=  4; break; } t0 = sptr[ 5]; sptr[ 5] = t1;
                if (t0 == ch) { idx +=  5; break; } t1 = sptr[ 6]; sptr[ 6] = t0;
                if (t1 == ch) { idx +=  6; break; } t0 = sptr[ 7]; sptr[ 7] = t1;
                if (t0 == ch) { idx +=  7; break; } t1 = sptr[ 8]; sptr[ 8] = t0;
                if (t1 == ch) { idx +=  8; break; } t0 = sptr[ 9]; sptr[ 9] = t1;
                if (t0 == ch) { idx +=  9; break; } t1 = sptr[10]; sptr[10] = t0;
                if (t1 == ch) { idx += 10; break; } t0 = sptr[11]; sptr[11] = t1;
                if (t0 == ch) { idx += 11; break; } t1 = sptr[12]; sptr[12] = t0;
                if (t1 == ch) { idx += 12; break; } t0 = sptr[13]; sptr[13] = t1;
                if (t0 == ch) { idx += 13; break; } t1 = sptr[14]; sptr[14] = t0;
                if (t1 == ch) { idx += 14; break; } t0 = sptr[15]; sptr[15] = t1;
                if (t0 == ch) { idx += 15; break; } t1 = sptr[16]; sptr[16] = t0;

                idx += 16;
                sptr += 16;
            }
            symbol[0] = ch;
        }
        *dst = idx;
    }
}

/* void mtf2(char *dst, char const *src, std::size_t len)
 * {
 *     char const *end = src + len;
 *
 *     // linked list: MTF symbol table and head
 *     // next[ch] = character after ch in MTF symbol table
 *     unsigned char next[256], firstChar = 0;
 *     // initialize list: next[0] = 1, next[1] = 2, ...
 *     std::iota(next, next + 255, 1);
 *
 *     for (; src != end; ++src, ++dst) {
 *         unsigned char ch = *src, t0, t1, idx;
 *
 *         idx = 0;
 *         if (firstChar != ch) {
 *             t1 = firstChar;
 *             // traverse the linked list
 *             while (true) {
 *                 if ((t0 = next[t1]) == ch) { ++idx;  next[t1] = next[ch]; break; }
 *                 if ((t1 = next[t0]) == ch) { idx+= 2; next[t0] = next[ch]; break; }
 *                 if ((t0 = next[t1]) == ch) { idx+= 3; next[t1] = next[ch]; break; }
 *                 if ((t1 = next[t0]) == ch) { idx+= 4; next[t0] = next[ch]; break; }
 *                 if ((t0 = next[t1]) == ch) { idx+= 5; next[t1] = next[ch]; break; }
 *                 if ((t1 = next[t0]) == ch) { idx+= 6; next[t0] = next[ch]; break; }
 *                 if ((t0 = next[t1]) == ch) { idx+= 7; next[t1] = next[ch]; break; }
 *                 if ((t1 = next[t0]) == ch) { idx+= 8; next[t0] = next[ch]; break; }
 *                 if ((t0 = next[t1]) == ch) { idx+= 9; next[t1] = next[ch]; break; }
 *                 if ((t1 = next[t0]) == ch) { idx+=10; next[t0] = next[ch]; break; }
 *                 if ((t0 = next[t1]) == ch) { idx+=11; next[t1] = next[ch]; break; }
 *                 if ((t1 = next[t0]) == ch) { idx+=12; next[t0] = next[ch]; break; }
 *                 if ((t0 = next[t1]) == ch) { idx+=13; next[t1] = next[ch]; break; }
 *                 if ((t1 = next[t0]) == ch) { idx+=14; next[t0] = next[ch]; break; }
 *                 if ((t0 = next[t1]) == ch) { idx+=15; next[t1] = next[ch]; break; }
 *                 if ((t1 = next[t0]) == ch) { idx+=16; next[t0] = next[ch]; break; }
 *
 *                 idx += 16;
 *             }
 *             next[ch] = firstChar;
 *             firstChar = ch;
 *         }
 *         *dst = idx;
 *     }
 * }
 */

/*
 * ===  FUNCTION  ======================================================================
 *         Name:  mtf_decode_block
 *  Description:
 *  \param dst: output string
 *  \param src: source string
 *  \param symbol: symbol table
 *  \param len: string len
 * =====================================================================================
 */
template <typename pcstring_type, typename pstring_type>
void mtf_decode_block(pstring_type dst, pcstring_type src, pstring_type symbol, std::size_t len)
{
    typedef typename std::iterator_traits<pstring_type>::value_type char_type;
    typedef typename std::make_unsigned<char_type>::type uchar_type;
    pcstring_type s_end = src + len;

    for (; src != s_end; ++src, ++dst) {
        uchar_type ch, idx = *src;
//        char_type t0, t1;

        ch = symbol[idx];
        if (idx != 0) {
            // shift symbol table
//            unsigned char *psb = symbol + idx - 1;
//            t0 = *psb;
//            while (psb >= symbol + 8) {
//                t1 = psb[-1]; psb[-1] = t0;
//                t0 = psb[-1]; psb[-1] = t1;
//
//                psb[-0] = t0; t1 = psb[-1];
//                psb[-1] = t1; t0 = psb[-2];
//                psb[-2] = t0; t1 = psb[-3];
//                psb[-3] = t1; t0 = psb[-4];
//                psb[-4] = t0; t1 = psb[-5];
//                psb[-5] = t1; t0 = psb[-6];
//                psb[-6] = t0; t1 = psb[-7];
//                psb[-7] = t1; t0 = psb[-8];
//                psb -= 8;
//            }
            std::memmove(symbol+1, symbol, idx);
            symbol[0] = ch;
        }
        *dst = ch;
    }
}

template <typename OStream, typename IStream>
void mtf_encode(OStream &os, IStream &is)
{
    typedef typename IStream::char_type char_type;
    static_assert(std::is_same<char_type, typename OStream::char_type>::value,
            "IStream and OStream must have same char_type");

    static const std::size_t alphabet_size = 1ULL << (8 * sizeof(char_type));
    char_type *src = std::get_temporary_buffer<char_type>(block_size).first,
              *dst = std::get_temporary_buffer<char_type>(block_size).first,
              *sym = std::get_temporary_buffer<char_type>(alphabet_size).first;

    //init symbol table
    std::iota(sym, sym + alphabet_size, 0);
    while (!is.eof()) {
        is.read(src, block_size);
        std::size_t len = is.gcount();
        mtf::mtf_encode_block(dst, src, sym, len);
        os.write(dst, len);
    }

    std::return_temporary_buffer(src);
    std::return_temporary_buffer(dst);
    std::return_temporary_buffer(sym);
}

template <typename pstring_type, typename pcstring_type>
void mtf_encode(pstring_type dst, pcstring_type src, std::size_t len)
{
    if (len == 0) return;
    if (len == 1) { *dst = *src; return; }

    typedef typename std::iterator_traits<pstring_type>::value_type char_type;

    //init symbol table
    static const std::size_t alphabet_size = 1ULL << (8 * sizeof(char_type));
    char_type *sym = std::get_temporary_buffer<char_type>(alphabet_size).first;
    std::iota(sym, sym + alphabet_size, 0);

    mtf::mtf_encode_block(dst, src, sym, len);

    std::return_temporary_buffer(sym);
}

template <typename OStream, typename IStream>
void mtf_decode(OStream &os, IStream &is)
{
    typedef typename IStream::char_type char_type;
    static_assert(std::is_same<char_type, typename OStream::char_type>::value,
                    "IStream and OStream must have same char_type");

    static const std::size_t alphabet_size = 1ULL << (8 * sizeof(char_type));
    char_type *src = std::get_temporary_buffer<char_type>(block_size).first,
              *dst = std::get_temporary_buffer<char_type>(block_size).first,
              *sym = std::get_temporary_buffer<char_type>(alphabet_size).first;

    //init symbol table
    std::iota(sym, sym + alphabet_size, 0);
    while (!is.eof()) {
        is.read(src, block_size);
        std::size_t len = is.gcount();
        mtf::mtf_decode_block(dst, src, sym, len);
        os.write(dst, len);
    }

    std::return_temporary_buffer(src);
    std::return_temporary_buffer(dst);
    std::return_temporary_buffer(sym);
}

template <typename pstring_type, typename pcstring_type>
void mtf_decode(pstring_type dst, pcstring_type src, std::size_t len)
{
    typedef typename std::iterator_traits<pstring_type>::value_type char_type;

    if (len == 0) return;
    if (len == 1) { *dst = *src; return; }

    //init symbol table
    static const std::size_t alphabet_size = 1ULL << (8 * sizeof(char_type));
    char_type *sym = std::get_temporary_buffer<char_type>(alphabet_size).first;
    std::iota(sym, sym + alphabet_size, 0);

    mtf::mtf_decode_block(dst, src, sym, len);

    std::return_temporary_buffer(sym);
}

}// namespace mtf

#endif // __MTF_HPP__
