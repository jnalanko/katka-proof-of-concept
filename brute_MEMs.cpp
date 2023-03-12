#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
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

int64_t lcp(const string& S, const string& T){
    int64_t len = 0;
    for(int64_t i = 0; i < min(S.size(), T.size()); i++){
        if(S[i] == T[i]) len++;
        else break;
    }
    return len;
}

int64_t length_of_longest_match(const string& S, const vector<string>& suffixes){
    int64_t ans = 0;
    for(const string& suf : suffixes){
        ans = max(ans, lcp(S, suf));
    }
    return ans;
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

    vector<string> suffixes;
    for(int64_t i = 0; i < n; i++)
        suffixes.push_back(ref.substr(i));
    std::sort(suffixes.begin(), suffixes.end());

    //for(int64_t i = 0; i < n; i++){
    //    cout << i << " " << ((i == 0) ? 0 : lcp(suffixes[i], suffixes[i-1])) << " " << suffixes[i] << endl;
    //}

    for(int64_t i = query.size()-1; i >= 0; i--){
        int64_t match_len = length_of_longest_match(query.substr(i), suffixes);
        int64_t left = 1e9; // "Infinity
        int64_t right = -1;
        for(int64_t j = 0; j < suffixes.size(); j++){
            if(lcp(suffixes[j], query.substr(i)) >= match_len){
                left = min(left, j);
                right = max(right, j);
            }
        }
        cout << left << " " << right+1 << " " << match_len << endl; // +1 to right to get exclusive end
    }


}