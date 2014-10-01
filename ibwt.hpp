/*
 * =====================================================================================
 *
 *       Filename:  ibwt.hpp
 *
 *    Description:  Inverse BWT
 *
 *        Version:  1.0
 *        Created:  09/30/2014 10:10:19 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  BOSS14420 (), firefoxlinux at gmail dot com
 *   Organization:
 *
 * =====================================================================================
 */

#ifndef __IBWT_HPP__
#define __IBWT_HPP__

#include <algorithm>
#include <iterator>
#include <utility>
#include <cstring>
#include <memory>
#include <type_traits>

namespace bwt {

/*
 * ===  FUNCTION  ======================================================================
 *         Name:  ibwt
 *  Description:  compute inverse BWT
 *  \param dst: output string
 *  \param bwt: transformed string
 *  \param n: string length
 *  \param k: alphabet size
 *  \param pidx: index of first suffix
 * =====================================================================================
 */
template <typename pcstring_type, typename pstring_type, typename index_type>
void
ibwt(pstring_type dst, pcstring_type bwt, index_type n, index_type k, index_type pidx)
{
    static_assert(std::is_signed<index_type>::value, "require signed index type");
    index_type *K = std::get_temporary_buffer<index_type>(k).first,
               *C = std::get_temporary_buffer<index_type>(n).first,
               *M = std::get_temporary_buffer<index_type>(k).first;

    // count
    std::memset(K, 0, k*sizeof(index_type));
    for (index_type i = 0; i < n; ++i) {
        C[i] = K[bwt[i]];
        ++K[bwt[i]];
    }

    index_type sum = 1;
    std::memset(M, 0, k*sizeof(index_type));
    for (index_type c = 0; c < k; ++c) {
        M[c] = sum;
        sum += K[c];
    }

    index_type i = 0;
    for (index_type j = n - 1; j >= 0; --j) {
        dst[j] = bwt[i];
        i = C[i] + M[bwt[i]];
//        if (i > pidx) --i;
        i -= (i > pidx);
    }

    std::return_temporary_buffer(K);
    std::return_temporary_buffer(C);
    std::return_temporary_buffer(M);
}

} // namespace bwt

#endif // __IBWT_HPP__
