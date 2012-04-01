/**
 * @file   eval_celex.cpp
 * @author Taesun Moon <tsmoon@tsmoon-macbook.local>
 * @date   Sun Apr 19 13:01:26 2009
 * 
 * @brief  
 * 
 * 
 */

#include <fstream>
#include <iostream>
#include <string>

#include <unistd.h>

#include "Eval.h"

using namespace std;

int main(int argc, char* argv[]) {
  if(argc < 2) {
    cerr << "USAGE:\t"
	 << argv[0] << " "
	 << "[-c CELEX_FILE | -r GOLD_FILE] [-m MODEL_OUTPUT] [-v]" << endl
	 << "\t-m:\tIf MODEL_OUTPUT is unspecified, defaults to stdin" << endl
	 << "\t-v:\tVerbose output. Streams model clusters to stdout" << endl
	 << endl;
    return 1;
  }

  ifstream if_celex;
  istream* is_model = &cin;
  bool not_cin = false, verbose = false;
  infl::EvalCelexStream* ec;
  int c = 0;

  while ((c = getopt (argc, argv, "c:m:r:v")) != -1) {
    switch (c) {
    case 'c': // training directory
      if_celex.open(optarg);
      if(!if_celex) {
	cerr << "Could not open " << optarg << endl;
	return 1;
      }
      ec = new infl::EvalCelexStream;
      break;
    case 'r':
      if_celex.open(optarg);
      if(!if_celex) {
	cerr << "Could not open " << optarg << endl;
	return 1;
      }
      ec = new infl::EvalClusterPerLine;
      break;
    case 'm':
      is_model = new ifstream(optarg);
      not_cin = true;
      if(! *is_model) {
	cerr << "Could not open " << optarg << endl;
	delete is_model;
	return 1;
      }
      break;
    case 'v':
      verbose = true;
      break;
    }
  }

  ec->verbose = verbose;
  ec->set_standard(if_celex, *is_model);
  ec->eval_cluster(*is_model);
  delete ec->vocabulary;
  if(not_cin)
    delete is_model;
}
