#include <cstdio>
#include "LCS.h"

namespace infl {

  inline void printLCSTable(VVLCS& vvlcs, UINT len1, UINT len2)
  {
    for(UINT i = 0, j; i < len1; i++) 
      {
	for(j = 0; j < len2 - 1; j++) 
	  {
	    fprintf(stderr,"%2d.",(int) vvlcs[i][j]);
	  }
	fprintf(stderr,"%2d\n",(int) vvlcs[i][j]);
      }
  }

  string getLCS(const string& s1, const string& s2)
  {
    LEN st1_len = s1.size() + 1, st2_len = s2.size() + 1;
  
    VUI vui(st2_len, 0);
    VVUI vvui(st1_len, vui);
  
    VLCS vlcs(st2_len, NOTHING);
    VVLCS vvlcs(st1_len, vlcs);

    vvlcs[0][0] = FINAL;
    //  vvui[0][1] = 1;

    for(UINT i = 1; i < st1_len; i++) 
      {
	for(UINT j = 1; j < st2_len; j++) 
	  {
	    if(s1[i-1] == s2[j-1])
	      {
		vvui[i][j] = vvui[i-1][j-1] + 1;
		vvlcs[i][j] = UP_LEFT;
#ifdef DEBUG
		printLCSTable(vvlcs, st1_len, st2_len);
#endif
	      }
	    else if(vvui[i-1][j] >= vvui[i][j-1]) 
	      {
		vvui[i][j] = vvui[i-1][j];
		vvlcs[i][j] = UP;	      
#ifdef DEBUG
		printLCSTable(vvlcs, st1_len, st2_len);
#endif
	      }
	    else 
	      {
		vvui[i][j] = vvui[i][j-1];
		vvlcs[i][j] = LEFT;	      
#ifdef DEBUG
		printLCSTable(vvlcs, st1_len, st2_len);
#endif
	      }
	  }
      }

    return getLCS(s1, s2, vvlcs, st1_len - 1, st2_len - 1);
  }

  string getLCS(const string& s1, const string& s2, VVLCS& vvlcs, UINT i, UINT j) 
  {
    if(i == 0 || j == 0)
      return "";

    if(UP_LEFT == vvlcs[i][j])
      {
	return getLCS(s1, s2, vvlcs, i-1, j-1) + s1[i-1];
      }
    else if(UP == vvlcs[i][j]) 
      {
	return getLCS(s1, s2, vvlcs, i-1, j);
      }
    else 
      {
	return getLCS(s1, s2, vvlcs, i, j-1);
      }
  }

  UINT lenLCS(const string& s1, const string& s2)
  {
    LEN st1_len = s1.size() + 1, st2_len = s2.size() + 1;
  
    VUI vui(st2_len, 0);
    VVUI vvui(st1_len, vui);
  
    VLCS vlcs(st2_len, NOTHING);
    VVLCS vvlcs(st1_len, vlcs);

    vvlcs[0][0] = FINAL;

    for(UINT i = 1; i < st1_len; i++) 
      {
	for(UINT j = 1; j < st2_len; j++) 
	  {
	    if(s1[i-1] == s2[j-1])
	      {
		vvui[i][j] = vvui[i-1][j-1] + 1;
		vvlcs[i][j] = UP_LEFT;
#ifdef JUNK
		printLCSTable(vvlcs, st1_len, st2_len);
#endif
	      }
	    else if(vvui[i-1][j] >= vvui[i][j-1]) 
	      {
		vvui[i][j] = vvui[i-1][j];
		vvlcs[i][j] = UP;	      
#ifdef JUNK
		printLCSTable(vvlcs, st1_len, st2_len);
#endif
	      }
	    else 
	      {
		vvui[i][j] = vvui[i][j-1];
		vvlcs[i][j] = LEFT;	      
#ifdef JUNK
		printLCSTable(vvlcs, st1_len, st2_len);
#endif
	      }
	  }
      }

    return lenLCS(s1, s2, vvlcs, st1_len - 1, st2_len - 1);
  }

  UINT lenLCS(const string& s1, const string& s2, VVLCS& vvlcs, UINT i, UINT j) 
  {
    if(i == 0 || j == 0)
      return 0;

    if(UP_LEFT == vvlcs[i][j])
      {
	return lenLCS(s1, s2, vvlcs, i-1, j-1) + 1;
      }
    else if(UP == vvlcs[i][j]) 
      {
	return lenLCS(s1, s2, vvlcs, i-1, j);
      }
    else 
      {
	return lenLCS(s1, s2, vvlcs, i, j-1);
      }
  }

  SCORE lcs_dist(const string& s1, const string& s2)
  {
    SCORE lcs_len = lenLCS(s1, s2);
    return 0.5 * lcs_len * (1.0/s1.size() + 1.0/s2.size());
  }

}
