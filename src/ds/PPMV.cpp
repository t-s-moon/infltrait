/**
 * @file   ppmv.cpp
 * @author Taesun Moon <tsmoon@wireless-128-62-123-101.public.utexas.edu>
 * @date   Sat Feb  7 19:00:50 2009
 * 
 * @brief  
 * 
 * 
 */

#include "ds/PPMV.h"

namespace infl {

//   std::ostream& operator<<(std::ostream& os, const BinaryRule& r) {
//     char c[1024];
//     string s = isnull(r.s1);
//     sprintf(c, "suf1:%s\tsuf2:%s\tcount:%d\tp_count:%d",
// 	    s.c_str(), r.s2.c_str(), r.count,
// 	    static_cast<unsigned int>((r.ppmv_pset)->size()));
//     os << c;
//     return os;
//   }
  
//   std::ostream& operator<<(std::ostream& os, const UnrestrictedRule& ur) {
//     os << "suffixes(count:" << ur.count
//        << " p_count:" << ur.ppmv_pset->size() << ")\t" ;
//     for(set<string>::const_iterator it = ur.suffixes->begin();
// 	it != ur.suffixes->end(); ++it)
//       os << *it << " ";
//     return os;
//   }
  
  std::ostream& operator<<(std::ostream& os, const PPMV<RuleBase>& p) {
//     os << "stem:" << p.stem << "\t" << *(p.r);
    os << "stem:" << p.stem << "\t";
    p.r->output_to_stream(os);
    return os;
  }

//   std::ostream& operator<<(std::ostream& os, const RULE_PMAP<>& rp) {
//     for(RULE_PMAP<>::const_iterator it = rp.begin();
// 	it != rp.end(); ++it) {
//       os << *(it->second) << std::endl;
//     }
//     return os;
//   }

//   std::ostream& operator<<(std::ostream& os, const PPMV_PVEC& pv) {
//     for(PPMV_PVEC::const_iterator it = pv.begin();
// 	it != pv.end(); ++it) {
//       os << **it << std::endl;
//     }
//     return os;
//   }
}
