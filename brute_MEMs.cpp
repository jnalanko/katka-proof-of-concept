#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>

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
            concat += "$" + line;
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

// Returns a half-open interval
pair<int64_t, int64_t> get_lex_range(const string& S, const vector<string>& suffixes){
    int64_t left = 1e9; // "Infinity"
    int64_t right = -1;
    for(int64_t j = 0; j < suffixes.size(); j++){
        if(lcp(suffixes[j], S) == S.size()){
            left = min(left, j);
            right = max(right, j);
        }
    }
    if(right == - 1){
        cerr << "Error getting lex range of " << S << endl;
        exit(1);
    }
    return {left, right};
}

// This program is supposed to print exactly the same output as MEMs.cpp
int main(int argc, char** argv){

    if(argc == 1){
        cerr << "Usage: " << argv[0] << " example/reference.txt example/query.txt" << endl;
        cerr << "As a third parameter can pass --lex-ranges or --doc-ranges." << endl;
        cerr << "Those options affect what is printed to the output. --lex-ranges is the default." << endl;
        return 1;
    }

    bool print_as_lex_ranges = true;
    if(argc == 4){
        if(argv[3] == string("--lex-ranges")){
            print_as_lex_ranges = true;
        } else if(argv[3] == string("--doc-ranges")){
            print_as_lex_ranges = false;
        } else{
            cerr << "Error parsing argument " << argv[3] << endl;
            return 1;
        }
    }

    string ref = concat_FASTA(argv[1]);
    string query = concat_FASTA(argv[2]);

    ref += '#';
    int64_t n = ref.size();

    vector<string> suffixes;
    for(int64_t i = 0; i < n; i++)
        suffixes.push_back(ref.substr(i));
    std::sort(suffixes.begin(), suffixes.end());

    for(int64_t i = query.size()-1; i >= 0; i--){
        int64_t match_len = length_of_longest_match(query.substr(i), suffixes);
        // We now have the longest exact match starting from i.
        // It's a MEM iff it can not be extended to the left.
        if(i == 0 || length_of_longest_match(query.substr(i-1), suffixes) < match_len + 1){
            // Can not be extended to the left, so we have a MEM.
        }

        if(print_as_lex_ranges){
            int64_t left, right;
            std::tie(left,right) = get_lex_range(query.substr(i, match_len), suffixes);
            cout << left << " " << right+1 << " " << match_len << endl; // +1 to right to get exclusive end
        } else{
            cerr << "Error: not implemented" << endl;
            return 1;
        }
    }
}

//for(int64_t i = 0; i < n; i++){
//    cout << i << " " << ((i == 0) ? 0 : lcp(suffixes[i], suffixes[i-1])) << " " << suffixes[i] << endl;
//}