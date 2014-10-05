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
static index_type huffman_wlength = 16;
static index_type block_size = 900000;
static index_type hm_threshold = 200000;
static index_type capacity = block_size * 2; //(min_run+1)/ min_run + 1;
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

        if (reed < hm_threshold) huffman_wlength = 8;
        else huffman_wlength = 16;

        // pass1: bwt -> s2 = bwt
//        index_type pidx;
        index_type pidx = bwt::sais_bwt(s2, s1, SA, reed, alphabet_size);
        // pass2: mtf -> s2 = mtf(bwt())
        std::swap(s1, s2);
        mtf::mtf_encode(s2, s1, reed);
        // pass3: rle -> s2 = rle(mtf(bwt()))
        std::swap(s1, s2);
//        index_type rle_len = rle::rle_encode(s2, s1, reed, min_run);
        index_type chr_len, rl_len;
        std::tie(chr_len, rl_len) = rle::rle_encode2(s2, s2+reed, s1, reed, min_run);
        // pass4: huffman
        std::swap(s1, s2);
        std::istringstream iss;
        iss.rdbuf()->pubsetbuf((char*)s1, chr_len);
        std::ostringstream oss;
        Huffman h1(iss, Huffman::Compress, huffman_wlength);
        h1.compress(oss);
        auto const &str1 = oss.str();
        oss.seekp(0, std::ios::end);
        index_type h1len = oss.tellp();

        // output rle_len
        os.write((char*)&chr_len, sizeof(pidx));
        os.write((char*)&rl_len, sizeof(pidx));
        // output pidx
        os.write((char*)&pidx, sizeof(pidx));
        // output encoded string
        //            os.write((char*)s2, rle_len);
        os.write((char*)&h1len, sizeof(pidx));
        os.write(str1.c_str(), h1len);
//        os.write((char*)s2, chr_len);
//        os.write((char*)(s2+reed), rl_len);

        iss.clear();
        iss.rdbuf()->pubsetbuf((char*)(s1+reed), rl_len);
        Huffman h2(iss, Huffman::Compress, huffman_wlength);
        oss.clear();
        oss.str(std::string());
        h2.compress(oss);
        auto const str2 = std::move(oss.str());
        oss.seekp(0, std::ios::end);
        index_type h2len = oss.tellp();
        os.write((char*)&h2len, sizeof(pidx));
        os.write(str2.c_str(), h2len);
    }
}

void decode(std::ostream& os, std::istream &is)
{
    while (!is.eof()) {
        index_type pidx, chr_len, rl_len, h1len, h2len;
        is.read((char*)&chr_len, sizeof(pidx));
        is.read((char*)&rl_len, sizeof(pidx));
        is.read((char*)&pidx, sizeof(pidx));

        is.read((char*)&h1len, sizeof(pidx));
        is.read((char*)s1, h1len);
//        is.read((char*)s1, chr_len+rl_len);
        index_type reed = is.gcount();
        if (reed == 0) break;

        // pass0.1: decode huffman for chr
        std::istringstream iss;
        iss.rdbuf()->pubsetbuf((char*)s1, reed);
        std::ostringstream oss;
        Huffman h1(iss, Huffman::Decompress);
        h1.decompress(oss);
        auto const &str1 = oss.str();
//        oss.seekp(0, std::ios::end);
//        auto hd1_len = oss.tellp();
        std::copy(str1.begin(), str1.end(), s2);

        // pass 0.2: decode huffman for run_length
        is.read((char*)&h2len, sizeof(pidx));
        is.read((char*)s1, h2len);
        iss.clear();
        iss.rdbuf()->pubsetbuf((char*)s1, is.gcount());
        Huffman h2(iss, Huffman::Decompress);
        oss.clear();
        oss.str(std::string());
        h2.decompress(oss);
        auto const &str2 = oss.str();
        std::copy(str2.begin(), str2.end(), s2+block_size);

        // pass1: decode rle -> s2 = mtf(bwt())
        std::swap(s1, s2);
//        index_type orig_len = rle::rle_decode(s2, (unsigned char*)str.c_str(), hd_len, min_run);
        index_type orig_len = rle::rle_decode2(s2, s1, chr_len, s1+block_size, rl_len, min_run);
//        index_type orig_len = rle::rle_decode2(s2, s1, chr_len, s1+chr_len, rl_len, min_run);
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
