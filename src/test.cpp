#include <iostream>
#include <sstream>
#include <string>

#include "ml/ChiSquareTest.h"

int main(int argc, char* argv[]) {
  infl::ChiSquare<double,infl::STAT_TEST_DEFAULT> cs;
  std::string line;
  char c[1024];
  while(std::cin.getline(c,1024)) {
    std::istringstream iss(c);
    size_t st[4];
    int i = 0;
    while(iss >> st[i++])
      ;
    std::cout << cs.evaluate_alt(st[0], st[1],st[2],st[3]) << std::endl;
    std::cout << "New values: ";
  }
}
