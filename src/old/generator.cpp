#include "generator.hpp"

void generate_forms
(ostream& os,
 EVALBIN& eb,
 vector<vector<string> >& vbag,
 vector<string>& sufs,
 vector<string>& prefs)
{
  for(UINT top = 0; top < vbag.size(); top++)
    {
      set<string> newstems;
      set<string> stems;
      for(UINT sub = 0; sub < vbag[top].size(); sub++)
	{
	  for(UINT i = 0; i < sufs.size(); i++)
	    {
	      stems.insert(strip_suffix(vbag[top][0], sufs[i]));
	    }
	}
      for(set<string>::iterator it = stems.begin(); it != stems.end(); it++)
	{
	  for(UINT i = 0; i < prefs.size(); i++)
	    {
	      stems.insert(strip_prefix(*it, prefs[i]));
	    }
	}
      for(set<string>::iterator it = stems.begin(); it != stems.end(); it++)
	{
	  newstems.insert(*it);
	  for(UINT i = 0; i < sufs.size(); i++)
	    {
	      newstems.insert(*it + sufs[i]);
	    }
	  for(UINT i = 0; i < prefs.size(); i++)
	    {
	      newstems.insert(prefs[i] + *it);
	    }
	}
      if(newstems.size() > 0)
	{
	  set<string>::iterator it = newstems.begin(), prev_it;
	  eb[*it] = new set<string>;
	  eb[*it]->insert(*it);
	  prev_it = it;
	  it++;
	  for(; it != newstems.end(); it++)
	    {
	      eb[*it] = eb[*prev_it];
	      eb[*it]->insert(*it);
	      prev_it = it;
	    }
	}
      copy(newstems.begin(),
	   newstems.end(),
	   ostream_iterator<string>(os, " "));
      os << endl;
    }
}
