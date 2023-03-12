#include <iostream>
#include <fstream>
#include <vector>
#include <cassert>
#include <memory>
#include <cmath>
#include <unordered_map>
#include <bit>
#include "RMQ_support.hh"
#include "Tree.hh"

using namespace std;

int main(int argc, char** argv){

    string tree_encoding = read_file(argv[1]);

    // Trim trailing whitespace
    while(tree_encoding.back() == ' ' || tree_encoding.back() == '\n') tree_encoding.pop_back();

    Tree T = make_unique<Tree_Loader>()->load_tree(tree_encoding);

    T.dfs_leaf_names_left_to_right(0);

}