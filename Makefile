.PHONY: MEMs brute_MEMs LCA_part tests

LCA_part:
	g++ LCA_part.cpp -I include -I ./sdsl-lite/include -I ./sdsl-lite/build/external/libdivsufsort/include -L ./sdsl-lite/build/lib -lsdsl -L ./sdsl-lite/build/external/libdivsufsort/lib/ -ldivsufsort -ldivsufsort64 -o LCA_part -std=c++20 -O3 -Wno-deprecated-declarations

MEMs:
	g++ MEMs.cpp -I include -I ./sdsl-lite/include -I ./sdsl-lite/build/external/libdivsufsort/include -L ./sdsl-lite/build/lib -lsdsl -L ./sdsl-lite/build/external/libdivsufsort/lib/ -ldivsufsort -ldivsufsort64 -o MEMs -std=c++20 -O3 -Wno-deprecated-declarations

brute_MEMs:
	g++ brute_MEMs.cpp -I include -I ./sdsl-lite/include -I ./sdsl-lite/build/external/libdivsufsort/include -L ./sdsl-lite/build/lib -lsdsl -L ./sdsl-lite/build/external/libdivsufsort/lib/ -ldivsufsort -ldivsufsort64 -o brute_MEMs -std=c++20 -O3

tests:
	g++ tests.cpp -I include -I ./sdsl-lite/include -I ./sdsl-lite/build/external/libdivsufsort/include -L ./sdsl-lite/build/lib -lsdsl -L ./sdsl-lite/build/external/libdivsufsort/lib/ -ldivsufsort -ldivsufsort64 -o tests -std=c++20 -g -Wno-deprecated-declarations