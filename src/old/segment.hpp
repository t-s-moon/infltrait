#ifndef SEGMENT_HPP
#define SEGMENT_HPP

#include "defs.hpp"
#include "algo.hpp"
#include "cluster.hpp"

void recurse_depth
(const string& search_string,	// 
 vector<string>& max_path,	// 
 vector<string> current_path,	// 
 UINT& max_depth,		// 
 UINT current_depth, 		// 
 set<string> list_of_fixes,	// 
 string (*combine)(const string&, const string&),
				// 
 Str2IdMap<WORDID>& s2i);	// 

vector<string> get_max_depth	// 
(const string& word,		// 
 const vector<string>& fixes,	// 
 UINT max_depth,		// 
 string (*strip)(const string&, const string&),
				// 
 string (*combine)(const string&, const string&),
				// 
 Str2IdMap<WORDID>& s2i);	// 

void cutoff_affixes
(vector<string>& topten_p,	// 
 vector<string>& topten_s,	// 
 Str2IdMap<WORDID>& s2i,	// 
 vector<string>& v_max_pref,	// 
 vector<string>& v_max_suf);	// 

#endif
