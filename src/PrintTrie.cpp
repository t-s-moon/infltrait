/**
 * @file   PrintTrie.cpp
 * @author Taesun Moon <tsmoon@wireless-128-62-138-250.public.utexas.edu>
 * @date   Tue Apr 28 18:17:02 2009
 * 
 * @brief  
 * 
 * 
 */

#include <iostream>
#include <sstream>
#include <string>

#include "ds/Trie.h"

using namespace std;

int main(int argc, char* argv[]) {
  istream* is; bool newstream = false;
  if(argc == 2) {
    is = new ifstream(argv[1]);
    newstream = true;
  } else {
    is = &cin;
  }

  const size_t BUFFSZ = 0xFFFF;
  char buffer[BUFFSZ];
  size_t counter = 0;
  while(is->getline(buffer, BUFFSZ)) {
    istringstream iss(buffer);
    string word;
    infl::Trie<> tr;
    iss >> word;
    tr.set_first(word);
    while(iss >> word)
      tr.insert(word);
    tr.graphviz_print(cout,counter);
    ++counter;
  }

  if(newstream)
    delete is;
}

