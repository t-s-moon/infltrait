#include "segment.hpp"

void recurse_depth
(const string& search_string,
 vector<string>& max_path,
 vector<string> current_path,
 UINT& max_depth,
 UINT current_depth, 
 set<string> list_of_fixes,
 string (*combine)(const string&, const string&),
 Str2IdMap<WORDID>& s2i)
{
  for(set<string>::iterator sit = list_of_fixes.begin();
      sit != list_of_fixes.end(); ++sit)
    {
      set<string> this_fixes;
      copy(list_of_fixes.begin(),
	   list_of_fixes.end(),
	   inserter(this_fixes, this_fixes.end()));
      this_fixes.erase(*sit);
      string this_search = combine(search_string, *sit);
      if(s2i.isMember(this_search))
	{
	  current_path.push_back(*sit);
	  if(current_depth > max_depth)
	    {
	      max_depth = current_depth;
	      max_path.clear();
	      copy(current_path.begin(),
		   current_path.end(),
		   inserter(max_path, max_path.end()));
#ifdef DEBUG
// 	      cerr << "NEW MAXDEPTH FOUND!" << endl;
// 	      copy(max_path.begin(), max_path.end(), ostream_iterator<string>(cerr, " "));
// 	      cerr << endl;
#endif
	    }
	  recurse_depth(search_string,
			max_path,
			current_path,
			max_depth,
			++current_depth,
			this_fixes,
			combine, s2i);
		    
	  current_path.pop_back();
	}
    }
}

vector<string> get_max_depth
(const string& word,
 const vector<string>& fixes,
 UINT max_depth,
 string (*strip)(const string&, const string&), 
 string (*combine)(const string&, const string&), 
 Str2IdMap<WORDID>& s2i)
{
  set<string> list_of_fixes;

  vector<string> max_path, current_path;
  UINT this_max_depth = max_depth, current_depth = 1;
  
  copy(fixes.begin(),
       fixes.end(),
       inserter(list_of_fixes, list_of_fixes.end()));

  for(set<string>::iterator it = list_of_fixes.begin();
      it != list_of_fixes.end(); it++)
    {
      set<string> this_fixes;
      copy(list_of_fixes.begin(),
	   list_of_fixes.end(),
	   inserter(this_fixes, this_fixes.end()));
      this_fixes.erase(*it);
      string stem = strip(word, *it);
      if(stem != word)
	{
	  current_path.push_back(*it);
	  if(!max_path.size())
	    max_path = current_path;
	  recurse_depth(stem,
			max_path,
			current_path,
			this_max_depth,
			++current_depth,
			this_fixes,
			combine, s2i);
	  current_path.pop_back();
	}
    }
  return max_path;
}


void cutoff_affixes
(vector<string>& topten_p,
 vector<string>& topten_s,
 Str2IdMap<WORDID>& s2i,
 vector<string>& v_max_pref,
 vector<string>& v_max_suf)
{
  UINT max_depth_pref = 0, max_depth_suf = 0;
  vector<string> words = s2i.vStr();
  for(vector<string>::iterator cit = words.begin();
      cit != words.end(); cit++)
    {
      vector<string> vt_max_pref =
	get_max_depth(*cit, topten_p,
		      max_depth_pref,
		      strip_prefix,
		      prefix, s2i);
      
      if(vt_max_pref.size() > max_depth_pref)
	{
	  v_max_pref = vt_max_pref;
	  max_depth_pref = v_max_pref.size();
	}
      
      vector<string> vt_max_suf =
	get_max_depth(*cit, topten_s,
		      max_depth_suf,
		      strip_suffix,
		      suffix, s2i);

      if(vt_max_suf.size() > max_depth_suf)
	{
	  v_max_suf = vt_max_suf;
	  max_depth_suf = v_max_suf.size();
	}
    }
}

