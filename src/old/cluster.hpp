#ifndef CLUSTER_HPP
#define CLUSTER_HPP

#include "lcs.hpp"
#include "ansi_ctype.hpp"
#include "algo.hpp"
#include "str2id.hpp"
#include "patricia.hpp"

typedef pair<PROB, PROB> SCORES;
typedef pair<UINT, string> COUNT_TO_WORD;
typedef vector<COUNT_TO_WORD> SORTING_BIN;
typedef map<WORDID, UINT> WORDCOUNT;
typedef map<WORDBAG, SCORES> DISTANCES;

typedef map<WORDID, WORDCOUNT> MWC;


typedef vector<WORDID> CLUSTERTABLE;
typedef vector<WORDBAG> CLUSTERBAG;

typedef map<WORDBAG, PROB> JWBAG;

typedef map<string, set<string>*> EVALBIN;

typedef map<char, WORDBAG> WORDBYPOS;

const double STHRESH2 = 0.9;
const double JTHRESH = 0.8;

class Cluster
{
private:
  string pos_tag;
public:
  bool operator<(const Cluster&);
};

class Clusters
{
};

void
print_beancounter(ostream& os,	             // Output stream
		  BEANCOUNTER& bc,	     // Beancounter
		  vector<string>& topten_p,  // 
		  vector<string>& topten_s); // 

#endif
