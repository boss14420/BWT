/*
 * =====================================================================================
 *
 *       Filename:  sais.hpp
 *
 *    Description:  SAIS algorithm for Suffix Array and BWT
 *
 *        Version:  1.0
 *        Created:  09/30/2014 10:03:15 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  BOSS14420 (), firefoxlinux at gmail dot com
 *   Organization:
 *
 * =====================================================================================
 */

#ifndef __SAIS_HPP__
#define __SAIS_HPP__

#include <cstring>
#include <memory>
#include <algorithm>
#include <cassert>
#include <vector>

namespace bwt {
namespace sais_private {
enum IdxType { LType = false, SType = true };

template <typename parray_type_bc, typename index_type>
void calc_bucket_end(parray_type_bc B, parray_type_bc const C, index_type k) {
    index_type sum = 0;
    for (index_type i = 0; i < k; ++i) sum += C[i], B[i] = sum;
}

template <typename parray_type_bc, typename index_type>
void calc_bucket_start(parray_type_bc B, parray_type_bc const C, index_type k) {
    index_type sum = 0;
    for (index_type i = 0; i < k; ++i) B[i] = sum, sum += C[i];
}

template <typename pstring_type, typename parray_type, typename parray_type_bc, typename index_type>
void induce_sa_ls(parray_type const SA, pstring_type const s, std::vector<bool> const &t,
                  parray_type_bc const C, parray_type_bc B, index_type n, index_type k)
{
    // induce L
    calc_bucket_start(B, C, k);
    index_type i, j;
    SA[B[s[n-1]]++] = n-1; // last char is TypeL
    for (index_type i = 0; i < n; ++i) {
        j = SA[i] - 1;
        // put j to next slot of its bucket
        if (j >= 0 && t[j] == LType) SA[B[s[j]]++] = j;
    }

    // induce S
    calc_bucket_end(B, C, k);
    for (i = n-1; i >= 0; --i) {
        j = SA[i] - 1;
        // put j to the end its bucket
        if (j >= 0 && t[j] == SType) SA[--B[s[j]]] = j;
    }
}

template <typename pstring_type, typename parray_type, typename index_type>
index_type
sais(parray_type SA, pstring_type const s, index_type n, index_type k, bool isbwt = false)
{
    index_type i, j;
    index_type *C = new index_type[k],
               *B = new index_type[k];

    // count
//    std::memset(C, 0, sizeof(index_type) * k);
    std::fill(C, C + k, 0);
    for (i = 0; i < n; ++i) ++C[s[i]];

    // type: true = S, false = L
    std::vector<bool> t(n);
    t[n-1] = LType;
    for (i = n - 2; i >= 0; --i)
        t[i] = (s[i] < s[i+1]) || (s[i] == s[i+1] && t[i+1] == SType);


    /////////////////////////////////////
    ///// stage 1: reduce to 1/2

    calc_bucket_end(B, C, k);

//    std::memset(SA, -1, sizeof(index_type) * n);
    std::fill(SA, SA + n, -1);
#define IS_LMS(i) (t[i-1] == LType && t[i] == SType)
    // put every LMS to end of its bucket
    for (i = 1; i < n-1; ++i)
        if (IS_LMS(i)) SA[--B[s[i]]] = i;

    // sort all LMS substrings
    induce_sa_ls(SA, s, t, C, B, n, k);

    delete[] B;

    // compact sorted LMS into first n1 elements of SA, n1 <= n/2
    index_type n1 = 0;
    for (i = 0; i < n; ++i)
        if (SA[i] > 0 && IS_LMS(SA[i])) SA[n1++] = SA[i];

    parray_type s1 = SA + n1;

    index_type last_lms = n-2;
    if (n1 > 0)
        while (!IS_LMS(last_lms)) --last_lms;

    // find lexicographic names of all LMS
//    std::memset(s1, -1, (n-n1) * sizeof(index_type));
    std::fill(s1, SA + n, -1);
    index_type name = (n1 > 0) ? 1 : 0; //! name = number of lexicographic names
    index_type prev = SA[0]; //! prev = position of previous LMS
    s1[prev/2] = 0;
    for (i = 1; i < n1; ++i) {
        index_type pos = SA[i]; // ith LMS
        bool diff = false; // ith LMS == (i-1)th LMS ?
        if (pos != last_lms && prev != last_lms) {
            for (index_type d = 0; d < n - pos; ++d) {
                if ((s[pos + d] != s[prev + d]) || (t[pos+d] != t[prev+d])) {
                    // dth characters of ith LMS different from dth characters of
                    // previous LMS
                    diff = true; break;
                }
                else if (IS_LMS(pos+d) && d > 0) {
                    // end of substring, two LMS substrings are equal
                    break;
                }
            }
        } else {
            diff = true;
        }
        if (diff) {++name; prev = pos; }
        s1[pos>>1] = name-1; // name of ith LMS = name-1
    }


    ////////////////////////////////////////////
    ///// STAGE 2 : recursion

    // last n1 elements of SA = s1
    s1 = SA + n - n1;
    for (i = n-1, j = n-1; j >= n-n1; --i)
//    for (i = n-1, j = n-1; i >= n1; --i)
        if (SA[i] >= 0) { SA[j--] = SA[i]; }

    parray_type SA1 = SA;
//    index_type *s1 = SA + n - n1, *SA1 = SA;
    if (name < n1) // if names is not unique, recursion
        sais(SA, s1, n1, name);
    else
        for (i = 0; i < n1; ++i) SA1[s1[i]] = i;


    /////////////////////////////////////////////
    ///// STAGE 3 : induce the result

    B = new index_type[k];
    calc_bucket_end(B, C, k);

    // s1 = LMS indexes of s
    for (i = 1, j = 0; i < n-1; ++i)
        if(IS_LMS(i)) s1[j++] = i;

    for(i=0; i <n1; ++i) SA1[i] = s1[SA[i]]; // SA1 = sorted LMS indexes
//    std::memset(SA + n1, -1, (n-n1)*sizeof(index_type));
    std::fill(SA + n1, SA + n, -1);
    index_type sidx;
    for (i = n1-1; i >= 0; --i) {
        sidx = SA[i]; SA[i] = -1;
        SA[--B[s[sidx]]] = sidx;
    }

    induce_sa_ls(SA, s, t, C, B, n, k);

    delete[] B;
    delete[] C;

#undef IS_LMS
    if (isbwt) {
        return std::find(SA, SA+n, 0) - SA;
    } else return 0;
}

} // sais_private


/*
 * sais
 * construct suffix array by SAIS algorithm
 * \param s: input string iterator (RandomAccessIterator)
 * \param SA: output suffix array iterator (RandomAccessIterator)
 * \param n: string length
 * \param k: alphabet size
 */
template <typename pstring_type, typename parray_type, typename index_type>
index_type
sais(parray_type SA, pstring_type const s, index_type n, index_type k)
{
    typedef typename std::iterator_traits<parray_type>::value_type sa_value;
    static_assert(std::is_signed<index_type>::value, "Require signed index type");
    static_assert(std::numeric_limits<sa_value>::max() == std::numeric_limits<index_type>::max(), "");
    assert(std::max(n, k) < std::numeric_limits<index_type>::max());

    if (n < 0) return -1;
    if (n == 0) return 0;
    if (n == 1) { SA[0] = 1; return 0; }
    return sais_private::sais(SA, s, n, k, false);
}

/*
 * sais_bwt
 * construct BWT by SAIS algorithm
 * \param s: input string iterator (RandomAccessIterator)
 * \param b: output bwt string iterator (RandomAccessIterator)
 * \param SA: suffix array iterator (RandomAccessIterator)
 * \param n: string length
 * \param k: alphabet size
 */
template <typename pcstring_type, typename pstring_type, typename parray_type, typename index_type>
index_type
sais_bwt(pstring_type b, pcstring_type s, parray_type SA, index_type n, index_type k)
{
    typedef typename std::iterator_traits<parray_type>::value_type sa_value;
    static_assert(std::is_signed<index_type>::value, "Require signed index type");
    static_assert(std::numeric_limits<sa_value>::max() == std::numeric_limits<index_type>::max(), "");
    assert(std::max(n, k) < std::numeric_limits<index_type>::max());

    if (n < 0) return -1;
    if (n == 0) return 0;
    if (n == 1) { b[0] = s[0]; return 0; }

    auto idx = sais_private::sais(SA, s, n, k, true);
    index_type i;
    b[0] = s[n-1];
    for (i = 0; i < idx; ++i) b[i+1] = s[SA[i]-1];
    for (i = idx + 1; i < n; ++i) b[i] = s[SA[i]-1];
    return idx;
}

} // namespace bwt

#endif // __SAIS_HPP__
