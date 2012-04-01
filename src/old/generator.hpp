#ifndef GENERATOR_HPP
#define GENERATOR_HPP

#include "defs.hpp"
#include "algo.hpp"

void generate_forms
(ostream& os,			// 
 EVALBIN& eb,			// 
 vector<vector<string> >& vbag,	// 
 vector<string>& sufs,		// 
 vector<string>& prefs);	// 

#endif
