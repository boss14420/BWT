all: bwt

CXX = g++
#CXXFLAGS = -Wall -std=c++11 -g -pipe
CXXFLAGS = -Wall -std=c++11 -O3 -march=native -funroll-loops -ftree-vectorize -pipe

bwt: bwt.o huffman.o bitstream.o
	$(CXX) $(CXXFLAGS) -o $@ $^

bwt.o: main.cc sais.hpp ibwt.hpp mtf.hpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

huffman.o: huffman2/huffman.cc huffman2/huffman.hh
	$(CXX) $(CXXFLAGS) -c -o $@ $<

bitstream.o: huffman2/bitstream.cc huffman2/bitstream.hh
	$(CXX) $(CXXFLAGS) -c -o $@ $<


.PHONY: clean

clean:
	rm -rf *.o
