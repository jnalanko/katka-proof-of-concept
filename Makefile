.PHONY: MEMs brute_MEMs

MEMs:
	g++ MEMs.cpp -I include -I ./sdsl-lite/include -I ./sdsl-lite/build/external/libdivsufsort/include -L ./sdsl-lite/build/lib -lsdsl -L ./sdsl-lite/build/external/libdivsufsort/lib/ -ldivsufsort -ldivsufsort64 -o MEMs -std=c++20 -O3 -Wno-deprecated-declarations

brute_MEMs:
	g++ brute_MEMs.cpp -I include -I ./sdsl-lite/include -I ./sdsl-lite/build/external/libdivsufsort/include -L ./sdsl-lite/build/lib -lsdsl -L ./sdsl-lite/build/external/libdivsufsort/lib/ -ldivsufsort -ldivsufsort64 -o brute_MEMs -std=c++20 -O3
