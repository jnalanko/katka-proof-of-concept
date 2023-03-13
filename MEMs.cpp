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
        return 1;
    }

    string ref = concat_FASTA(argv[1]);
    ref += '#';

    string query = concat_FASTA(argv[2]);
    

    cerr << "Indexing" << endl;
    MEM_Finder finder(ref);

    cerr << "Computing MEMs" << endl;
    vector<MEM_Finder::MEM> MEMs = finder.run(query);

    cerr << "Locating leftmost and rightmost document for each MEM" << endl;
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