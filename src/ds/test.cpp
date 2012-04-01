/**
 * @file   test.cpp
 * @author Taesun Moon <tsmoon@wireless-128-62-236-156.public.utexas.edu>
 * @date   Mon Mar  2 16:14:12 2009
 * 
 * @brief  
 * 
 * 
 */

#include <typeinfo>

#include "io/IO.h"

#include "ds/TermByDoc.h"
#include "ds/RuleByDoc.h"

#include "ml/kmeans.h"

using namespace std;

template <typename T1, typename T2>
double vecmul(const vector<T1>& v1, const vector<T2>& v2) {
  double sum = 0;
  for(size_t i = 0; i < v1.size(); ++i) {
    sum += v1.at(i) * v2.at(i);
  }
  return sum;
}

template <typename T>
class A {
public:
  typedef typename std::vector<T> VT;
};

template <typename T>
class B :public A<T> {
public:
  typedef typename std::map<T,T> VT;
};

int main(int argc, char* argv[]) {

  A<double>::VT vta;
  B<int>::VT vtb;

  std::cout << typeid(vta).name() << std::endl;
  std::cout << typeid(vtb).name() << std::endl;
  infl::StringFilter* sf = new infl::EnglishFilter;
  infl::TermByDocSparse tbda(sf, argv[1]);
  tbda.build_matrix();
  infl::KMeansSparse<size_t> km(tbda);
  km.cluster();

  km.print(std::cout);


//   infl::RuleByDoc rbd(sf, argv[1], 4);

//   rbd.process_rule_by_doc();
//   infl::RuleByDoc::RPSZ_PAIRVEC rpsz;
//   rbd.extract_top_n(&rpsz, 300, infl::RuleByDoc::COUNT);
//   infl::RULE_PVECPMAP& grp = rbd.get_rp();
  
//   for(size_t i = 0; i < rpsz.size(); ++i) {
//     infl::rule* r = rpsz[i].first;
//     std::cout << *r << std::endl;
//     for(infl::RULE_PVEC::iterator it = grp[*r]->begin();
// 	it != grp[*r]->end(); ++it) {
//       for(infl::PPMV_PSET::iterator pit = (*it)->ppmv_pset->begin();
// 	  pit != (*it)->ppmv_pset->end(); ++pit) {
// 	std::cout << **pit << "\t";
//       }
//       std::cout << std::endl;
//     }
//     std::cout << "\tcount: " << rpsz[i].second << std::endl;
//   }
}
