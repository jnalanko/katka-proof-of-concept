MEMs:
	g++ MEMs.cpp -I include -I ./sdsl-lite/include -I ./sdsl-lite/build/external/libdivsufsort/include -L ./sdsl-lite/build/lib -lsdsl -L ./sdsl-lite/build/external/libdivsufsort/lib/ -ldivsufsort -ldivsufsort64 -o example -std=c++14 -O3
