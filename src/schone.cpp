/**
 * @file   schone.cpp
 * @author Taesun Moon <tsmoon@wireless-128-62-123-101.public.utexas.edu>
 * @date   Fri Feb  6 22:15:47 2009
 * 
 * @brief  
 * 
 * 
 */

#include "io/NamedMatrix.h"

int main(int argc, const char* argv[]) {
  if(argc > 1) {
    infl::NamedMatrix* nm =
      new infl::NamedMatrix(argv[1]);
    nm->ppmv_stem_min = 2;
    nm->readstream();
    nm->calculate_semantics();
    nm->test_print();
  }
}
