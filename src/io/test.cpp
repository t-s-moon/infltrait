/**
 * @file   test.cpp
 * @author Taesun Moon <tsmoon@wireless-128-62-236-156.public.utexas.edu>
 * @date   Mon Mar  2 16:14:12 2009
 * 
 * @brief  
 * 
 * 
 */

#include "io/IO.h"
#include "io/TermByDoc.h"

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

int main(int argc, char* argv[]) {
  infl::StringFilter* sf = new infl::EnglishFilter;
  infl::TermByDoc td(sf, argv[1]);
  td.build_matrix();
  const infl::TermByDoc::MATRIX& m = td.get_matrix();
  const infl::Str2IdMap<size_t>& s2i = td.get_s2i();

  size_t colsize = td.get_colsize();
  vector<double> vd(colsize, sqrt(1.0 / colsize));

  infl::KMeans<double> km;

  for(size_t i = 0; i < m.size(); ++i) {
    std::string s = s2i.getStr(i);
    double vp = vecmul(vd, *m.at(i));
    km.add_vector(new vector<double>(1, vp), s);
  }

  km.cluster();

  const infl::KMeans<double>::KVECL& kl = km.get_clusters();
  const infl::KMeans<double>::KVECT& kt = km.get_labels();

  for(infl::KMeans<double>::KVECL::const_iterator it = kl.begin();
      it != kl.end(); ++it) {
    std::cout << it->second << "\t"
	      << kt.find(it->first)->second << std::endl;
  }
  
//   std::locale loc; const infl::AnsiCType* ct;
//   loc = std::locale::locale(std::locale::classic(), new infl::AnsiCType);
//   ct = &(std::use_facet<infl::AnsiCType>(loc));
//   unsigned char a = 'a';
//   unsigned char A = 'A';
//   unsigned char c = 224;
//   unsigned char C = 192;
//   std::cout << a << std::endl;
//   std::cout << A << std::endl;
//   std::cout << c << std::endl;
//   std::cout << C << std::endl;
//   if(ct->is(std::ctype_base::alpha, a))
//     std::cout << a << std::endl;
//   if(ct->is(std::ctype_base::alpha, A))
//     std::cout << A << std::endl;
//   if(ct->is(std::ctype_base::alpha, c))
//     std::cout << c << std::endl;
//   if(ct->is(std::ctype_base::alpha, C))
//     std::cout << C << std::endl;
//   std::cout << (unsigned int) ct->aclower(a) << std::endl;
//   std::cout <<(unsigned int)ct->acupper(a) << std::endl;
//   std::cout <<(unsigned int)ct->aclower(A) << std::endl;
//   std::cout <<(unsigned int)ct->acupper(A) << std::endl;
//   std::cout <<(unsigned int)ct->aclower(c) << std::endl;
//   std::cout <<(unsigned int)ct->acupper(c) << std::endl;
//   std::cout <<(unsigned int)ct->aclower(C) << std::endl;
//   std::cout <<(unsigned int)ct->acupper(C) << std::endl;
//   std::cout <<(unsigned int)argv[1] << std::endl;
}
