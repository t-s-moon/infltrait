#include "kmeans.h"

template<typename K>
void print(const std::vector<K>* const v) {
  std::copy(v->begin(), v->end(),
	    std::ostream_iterator<K>(std::cerr, ", "));
  std::cerr << std::endl;
}

int main(int argc, char* argv[]) {
  std::vector<std::vector<int>*> vvip;
  for(size_t i = 0; i < 30; ++i) {
    std::vector<int>* vi = new std::vector<int>;
    vvip.push_back(vi);
    for(size_t j = 0; j < 2; ++j) {
      vi->push_back(rand() % 10);
    }
    //    print(vi);
  }
  infl::kmeans<int> km(2);
  for(size_t i = 0; i < vvip.size(); ++i) {
    km.add_vector(vvip[i]);
  }
  km.cluster();
  std::cerr << km;
  for(size_t i; i < vvip.size(); ++i) {
    delete vvip[i];
  }
}
