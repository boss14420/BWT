/*
 * =====================================================================================
 *
 *       Filename:  main.cc
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  09/30/2014 10:52:54 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  BOSS14420 (), firefoxlinux at gmail dot com
 *   Organization:
 *
 * =====================================================================================
 */


#include <iostream>
#include <memory>
#include <sstream>
#include <fstream>
#include "sais.hpp"
#include "ibwt.hpp"
#include "mtf.hpp"
#include "rle.hpp"
#include "huffman2/huffman.hh"

enum Mode {COMPRESS, DECOMPRESS, INVALID};

typedef std::int32_t index_type;
static index_type min_run = 3;
static index_type huffman_wlength = 8;
static index_type block_size = 900000;
static index_type capacity = block_size * (min_run+1)/ min_run + 1;
static index_type alphabet_size = 256;

static unsigned char *s1;
static unsigned char *s2;
static index_type *SA;

void encode(std::ostream& os, std::istream &is)
{
    while (!is.eof()) {
        is.read((char*)s1, block_size);
        index_type reed = is.gcount();
        if (reed == 0) break;

        // pass1: bwt -> s2 = bwt
        index_type pidx = bwt::sais_bwt(s2, s1, SA, reed, alphabet_size);
        // pass2: mtf -> s2 = mtf(bwt())
        std::swap(s1, s2);
        mtf::mtf_encode(s2, s1, reed);
        // pass3: rle -> s2 = rle(mtf(bwt()))
        std::swap(s1, s2);
        index_type rle_len = rle::rle_encode(s2, s1, reed, min_run);
        // pass4: huffman
        std::swap(s1, s2);
        std::istringstream iss;
        iss.rdbuf()->pubsetbuf((char*)s1, rle_len);
        std::ostringstream oss;
        Huffman h(iss, Huffman::Compress, huffman_wlength);
        h.compress(oss);
        auto const &str = oss.str();
        oss.seekp(0, std::ios::end);
        index_type hlen = oss.tellp();

        // output rle_len
        os.write((char*)&rle_len, sizeof(pidx));
        // output pidx
        os.write((char*)&pidx, sizeof(pidx));
        // output encoded string
        //            os.write((char*)s2, rle_len);
        os.write((char*)&hlen, sizeof(pidx));
        os.write(str.c_str(), hlen);
    }
}

void decode(std::ostream& os, std::istream &is)
{
    while (!is.eof()) {
        index_type pidx, rle_len, hlen;
        is.read((char*)&rle_len, sizeof(pidx));
        is.read((char*)&pidx, sizeof(pidx));
        is.read((char*)&hlen, sizeof(pidx));
        is.read((char*)s1, hlen);
        index_type reed = is.gcount();
        if (reed == 0) break;

        // pass0: decode huffman
        std::istringstream iss;
        iss.rdbuf()->pubsetbuf((char*)s1, reed);
        std::ostringstream oss;
        Huffman h(iss, Huffman::Decompress);
        h.decompress(oss);
        auto const &str = oss.str();
        oss.seekp(0, std::ios::end);
        auto hd_len = oss.tellp();

        // pass1: decode rle -> s2 = mtf(bwt())
        index_type orig_len = rle::rle_decode(s2, (unsigned char*)str.c_str(), hd_len, min_run);
        // pass2: decode mtf -> s2 = bwt
        std::swap(s1, s2);
        mtf::mtf_decode(s2, s1, orig_len);
        // pass3: ibwt -> s2 = original string
        std::swap(s1, s2);
        bwt::ibwt(s2, s1, orig_len, alphabet_size, pidx);

        // output encoded string
        os.write((char*)s2, orig_len);
    }

}

int main(int argc, char *argv[])
{
    if (argc < 4) return -1;
    Mode mode = INVALID;
    if (argv[1][0] == 'c') mode = COMPRESS;
    else if(argv[1][0] == 'x') mode = DECOMPRESS;
    else return -2;

    s1 = std::get_temporary_buffer<unsigned char>(capacity).first;
    s2 = std::get_temporary_buffer<unsigned char>(capacity).first;
    SA = std::get_temporary_buffer<index_type>(block_size).first;

    std::ostream *os;
    std::istream *is;

    std::string hyphen("-");

    if (argv[2] != hyphen) {
        os = new std::ofstream(argv[2],
                std::ios::out | std::ios::binary);
        if (!*os) {
            std::cerr << "Cannot open " << argv[2] << "\n";
            return -3;
        }
    } else os = &std::cout;

    if (argv[3] != hyphen) {
        is = new std::ifstream(argv[3],
                std::ios::in | std::ios::binary);
        if (!*is) {
            std::cerr << "Cannot open " << argv[2] << "\n";
            if (argv[2] != hyphen) delete os;
            return -3;
        }
    } else is = &std::cin;

    if (mode == COMPRESS) {
        encode(*os, *is);
    } else if (mode == DECOMPRESS) {
        decode(*os, *is);
    }

    std::return_temporary_buffer(s1);
    std::return_temporary_buffer(s2);
    std::return_temporary_buffer(SA);

    if (argv[2] != hyphen) delete os;
    if (argv[3] != hyphen) delete is;

    return 0;
}
