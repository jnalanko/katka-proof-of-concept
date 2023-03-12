#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "divsufsort64.h"
#include "sdsl/wavelet_trees.hpp"
#include <sdsl/construct.hpp>

using namespace std;

string concat_FASTA(const string& filename){
    ifstream in(filename);
    if(!in.good()){
        cerr << "Error reading file: " << filename << endl;
        exit(1);
    }
    string concat;
    string line;
    while(getline(in, line)){
        if(line.size() > 0 && line[0] != '>')
            concat += line;
    }
    return concat;
}

void compute_MEMs(const vector<int64_t>& SA, const vector<int64_t>& LCP, const sdsl::wt_huff<>& BWT_wt, const vector<int64_t>& C, const string& query){
    // Backward search the query and use the LCP array to drop characters from the right
    int64_t n = SA.size();

    int64_t left = 0;
    int64_t right = n; // One past the end
    
    int64_t match_length = 0;
    for(int64_t i = query.size()-1; i >= 0; i--){
        char c = query[i];
        int64_t rankleft = BWT_wt.rank(left, c);
        int64_t rankright = BWT_wt.rank(right, c);

        bool MEM_flag = true;
        while(rankleft == rankright){ // No c to the left

            if(MEM_flag){
                // Print the MEM
                cout << left << " " << right << " " << match_length << endl;
                MEM_flag = false;
            }

            if(match_length == 0) break; // c does not occur in the BWT at all

            // Need to expand the interval
            match_length -= 1;
            while(left > 0 && LCP[left] >= match_length) left--;
            while(right-1 < n-1 && LCP[right] >= match_length) right++;
        
            rankleft = BWT_wt.rank(left, c);
            rankright = BWT_wt.rank(right, c);
        }

        // Do the FM index step
        left = C[c] + BWT_wt.rank(left, c);
        right = C[c] + BWT_wt.rank(right, c);

        match_length++;

        
    }
}

int main(int argc, char** argv){

    if(argc == 1){
        cerr << "Usage: " << argv[0] << " example/reference.txt example/query.txt" << endl;
        return 1;
    }

    string ref = concat_FASTA(argv[1]);
    string query = concat_FASTA(argv[2]);

    ref += '$';
    int64_t n = ref.size();

    // Compute the suffix array
    cerr << "Building SA" << endl;
    vector<int64_t> SA(n);
    divsufsort64((sauchar_t*)(&ref[0]), SA.data(), n);

    cerr << "Building ISA" << endl;
    // Compute the inverse suffix array
    vector<int64_t> ISA(n);
    for(int64_t i = 0; i < n; i++){
        ISA[SA[i]] = i;
    }

    // Build the LCP array
    cerr << "Building LCP" << endl;
    vector<int64_t> LCP(n);

    int64_t match_length = 0;
    for(int64_t i = 0; i < n; i++){
        match_length = max((int64_t) 0, match_length - 1); // Drop one character
        int64_t lex = ISA[i];
        if(lex > 0){
            int64_t j = SA[lex-1];
            while(i + match_length < n && j + match_length < n){
                if(ref[i + match_length] == ref[j + match_length]){
                    match_length++;
                } else break; // Mismatch
            }
        } else{
            match_length = 0;
        }
        LCP[lex] = match_length;
    }

    // Build the BWT
    cerr << "Building BWT" << endl;
    string BWT(n, ' ');
    for(int64_t i = 0; i < n; i++){
        int64_t textpos = SA[i];
        char c = textpos == 0 ? ref[n-1] : ref[textpos-1];
        BWT[i] = c;
    }

    cerr << "Building BWT wavelet tree" << endl;
    sdsl::wt_huff<> BWT_wt;
    sdsl::construct_im(BWT_wt, BWT.data(),  1);

    cerr << "Building C array" << endl;
    vector<int64_t> counts(256);
    for(char c : ref) counts[c]++;
    vector<int64_t> C(256); // Cumulative counts
    for(int64_t i = 1; i < 256; i++){
        C[i] = counts[i-1] + C[i-1];
    }

    compute_MEMs(SA, LCP, BWT_wt, C, query);

}