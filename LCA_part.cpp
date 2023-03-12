#include <iostream>
#include <fstream>
#include <vector>
#include <cassert>
#include <memory>
#include <cmath>
#include <sstream>
#include <unordered_map>
#include <bit>
#include "LCA_support.hh"
#include "Tree.hh"

using namespace std;

struct MEM{
    int64_t left_doc;
    int64_t right_doc;
    int64_t length;
};

vector<MEM> load_MEM_data(const string& filename){
    ifstream in(filename);
    if(!in.good()){
        cerr << "Error loading file: " << filename << endl;
        exit(1);
    }

    vector<MEM> mems;
    string line;
    while(getline(in, line)){
        stringstream ss(line);
        MEM mem;
        ss >> mem.length >> mem.left_doc >> mem.right_doc;
        mems.push_back(mem);
    }

    return mems;
}

int main(int argc, char** argv){

    string tree_encoding = read_file(argv[1]);
    vector<MEM> MEM_data = load_MEM_data(argv[2]);

    // Trim trailing whitespace
    while(tree_encoding.back() == ' ' || tree_encoding.back() == '\n') tree_encoding.pop_back();

    shared_ptr<Tree> T = make_unique<Tree_Loader>()->load_tree(tree_encoding);
    LCA_support LCA(T);

    for(MEM mem : MEM_data){
        if(mem.length > 20){
            int64_t lca = LCA.LCA(mem.left_doc, mem.right_doc);
            cout << lca << endl;
        }
    }

}