#ifndef __LCS_H__
#define __LCS_H__

#include <string>
#include <vector>

#include "Defs.h"

namespace infl {

  using std::string;
  
  enum LCSMarker {NOTHING, UP, LEFT, UP_LEFT, FINAL};

  typedef std::vector<LCSMarker> VLCS;
  typedef std::vector<VLCS> VVLCS;
  typedef std::vector<UINT> VUI;
  typedef std::vector<VUI> VVUI;

  string getLCS(const string&, const string&);
  string getLCS(const string&, const string&, VVLCS&, UINT, UINT);
  UINT lenLCS(const string&, const string&);
  UINT lenLCS(const string&, const string&, VVLCS&, UINT, UINT);
  SCORE lcs_dist(const string&, const string&);

}
#endif
