#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "MEMs.hh"

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
    ref += '#';

    string query = concat_FASTA(argv[2]);

    cerr << "Indexing" << endl;
    MEM_Finder finder(ref);

    cerr << "Computing MEMs" << endl;
    vector<MEM_Finder::MEM> MEMs = finder.run(query);

    if(print_as_lex_ranges){
        cerr << "Printing match lengths and their inclusive lex ranges" << endl;
        for(MEM_Finder::MEM mem : MEMs){
            cout << mem.match_length << " " << mem.lex_start << " " << mem.lex_end-1 << endl;
        }
    } else{
        cerr << "Printing match lengths and their inclusive doc ranges" << endl;
        for(MEM_Finder::MEM mem : MEMs){
            int64_t leftmost_doc = 1e18;
            int64_t rightmost_doc = -1;
            for(int64_t i = mem.lex_start; i < mem.lex_end; i++){
                leftmost_doc = min(leftmost_doc, finder.DA[i]);
                rightmost_doc = max(rightmost_doc, finder.DA[i]);
            }
            cout << mem.match_length << " " << leftmost_doc << " " << rightmost_doc << endl;
        }
    }
}