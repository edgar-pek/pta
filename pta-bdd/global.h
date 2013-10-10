#ifndef GLOBAL_H
#define GLOBAL_H

#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <deque>
#include <fstream>
#include <iostream>
#include <list>
#include <map>
#include <queue>
#include <set>
#include <sstream>
#include <string>
//#include <unordered_map>
#include <vector>
// llvm
#include <llvm/ADT/SparseBitVector.h>
using llvm::SparseBitVector;
using llvm::SparseBitVectorElement;

#include <fdd.h>

using std::deque;
using std::list;
using std::map;
using std::multimap;
using std::queue;
using std::set;
using std::vector;
using std::string;
using std::pair;
using std::make_pair;

using std::istringstream;
using std::stringstream;
using std::ifstream;
using std::ofstream;
using std::ostream;
using std::ios;
using std::cout;
using std::cerr;
using std::endl;

typedef unsigned uint;
typedef SparseBitVector< 4096 > BitVector;

//void print_contradiction( string s )
//{
//	cerr << "found contradiction when adding : " << s << endl;
//}

//vector< uint > decoded_heap_elements;
//void decode_heap_domain(char *hset, int size)
//{ }
#endif
