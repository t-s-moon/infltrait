#include "cluster.hpp"

void print_beancounter(ostream& os, BEANCOUNTER& bc, vector<string>& topten_p, vector<string>& topten_s)
{
  for(BEANCOUNTER::iterator it1 = bc.begin();
      it1 != bc.end();
      ++it1)
    {
      SORTING_BIN sb;
      for(map<string, UINT>::iterator it2 = (it1->second).begin();
	  it2 != (it1->second).end();
	  it2++)
	{
	  COUNT_TO_WORD sts = std::make_pair(it2->second, it2->first);
	  sb.push_back(sts);
	}

      os << "========================================" << endl;
      os << "PRINTING " << morph_name(it1->first) << endl;

      std::sort(sb.begin(), sb.end(), greater<COUNT_TO_WORD>());
      UINT ui = 0;
      for(SORTING_BIN::iterator it = sb.begin(); it != sb.end(); it++, ui++)
	{
	  os << it->second << " " << it->first << endl;
	  if(it1->first == PREFIX && ui < 10)
	    topten_p.push_back(it->second);
	  if(it1->first == SUFFIX && ui < 10)
	    topten_s.push_back(it->second);
	}
    }
}

void split_line(string& input, string& s1, string& s2, PROB& p)
{
  string::size_type beg_idx, end_idx;
  beg_idx = 0;
  end_idx = input.find(" ", beg_idx);
  s1 = input.substr(beg_idx, end_idx - beg_idx);
  beg_idx = end_idx + 1;
  end_idx = input.find(" ", beg_idx);
  s2 = input.substr(beg_idx, end_idx - beg_idx);
  if(!s1.size() || !s2.size())
    {
      cerr << "The line \"" << input << "\" has a blank in it!" << endl;
    }
  beg_idx = end_idx + 1;
  p = atof(input.substr(beg_idx).c_str());
}




// int main(int argc, char* argv[])
// {

//   if(argc < 6)
//     {
//       cerr << "Usage: " << *argv 
// 	   << " SOURCE_TO_TARGET TARGET_TO_SOURCE TRG_STOPWORDS POS_DICT OUTPUT STANDARD (THRESHOLDS)" << endl;
//       return 1;
//     }

//   ifstream source_to_target(*++argv);
//   ifstream target_to_source(*++argv);
//   ifstream if_pos_dict(*++argv);
//   ifstream if_trg_stop(*++argv);

//   string fname(*++argv);

//   ifstream if_celex(*++argv);

//   double threshold = THRESH,
//     sthresh1 = STHRESH1, 
//     sthresh2 = STHRESH2,
//     jthresh = JTHRESH;

//   if(argc > 7)
//     {
//       threshold = atof(*++argv);
//       sthresh1 = atof(*++argv);
//       sthresh2 = atof(*++argv);
//       jthresh = atof(*++argv);
//     }

//   if(!source_to_target || !target_to_source || !if_trg_stop)
//     {
//       cerr << "One of the files has failed to open." << endl;
//       return 1;
//     }

//   ifstream& st = source_to_target, &ts = target_to_source;
  
//   MWB mwb;
//   MWC mwc_clusters;
//   MWP pos_dict;

//   locale loc = locale(locale::classic(), new AnsiCType);
//   const ctype<char>& ct = use_facet<ctype<char> >(loc);

//   string line, target, source;
//   Str2IdMap<WORDID> s2i_source, s2i_target, s2i_verb, s2i_noun, s2i_adjs;
//   WORDBAG target_words1, target_words2;
//   SWORDBAG swb_trg_stop;
//   PROB p;

//   string prev_idx = "", prev_entry = "";
//   EVALBIN celex, mybin_verb, mybin_noun, mybin_adjs;
//   while(getline(if_celex, line))
//     {
//       vector<string> celex_line = split(line, "\\");
//       string entry = ct.tolower(celex_line[1][0]) + celex_line[1].substr(1, celex_line[1].size());
//       string this_idx = celex_line[3];
//       if(prev_idx != this_idx)
// 	{
// 	  celex[entry] = new set<string>;
// 	  celex[entry]->insert(entry);
// 	}
//       else
// 	{
// 	  celex[entry] = celex[prev_entry];
// 	  celex[entry]->insert(entry);
// 	}
//       prev_entry = entry;
//       prev_idx = this_idx;
//     }

// #ifdef DEBUG
//    cout << "CELEX entered!" << endl;
// //   cin.get();
// #endif
//   string word;
//   while(if_trg_stop >> word)
//     {
//       swb_trg_stop.insert(word);
//     }

//   string token, pos;
//   UINT count;
//   while(if_pos_dict >> token >> pos >> p >> count)
//     {
//       if(is_alpha(target, ct) 
// 	 && swb_trg_stop.find(target) == swb_trg_stop.end())
// 	{
// 	  pos_dict[s2i_target.getId(token)][pos[0]] += p;
// 	}
//     }

// //   for(MWP::iterator it = pos_dict.begin(); it != pos_dict.end(); it++)
// //     {
// //       char maxpos  = '\0';
// //       PROB maxposp = 0;
// //       for(WORDPROB::iterator itp = (it->second).begin(); itp != (it->second).end(); itp++)
// // 	{
// // 	  if(itp->second > maxposp)
// // 	    {
// // 	      maxpos = itp->first;
// // 	      maxposp = itp->second;
// // 	    }
// // 	}
// //       if(maxpos == 'V')
// // 	s2i_verb.getId(s2i_target.getStr(it->first));
// //       else if(maxpos == 'N')
// // 	s2i_noun.getId(s2i_target.getStr(it->first));
// //       else if(maxpos == 'J')
// // 	s2i_adjs.getId(s2i_target.getStr(it->first));
// //     }

//   while(getline(st, line))
//     {
//       split_line(line, target, source, p);
//       if(source != "NULL" && is_alpha(get_word(source), ct) && is_alpha(target, ct) 
// 	 && swb_trg_stop.find(target) == swb_trg_stop.end()
// 	 && p > threshold)
// 	{
// 	  WORDID wis = s2i_source.getId(source);
// 	  WORDID wit = s2i_target.getId(target);

// 	  mwb[wis].insert(wit);
// 	  target_words1.insert(wit);
// 	}
//     }

//   while(getline(ts, line))
//     {
//       split_line(line, source, target, p);
//       if(source != "NULL" && is_alpha(get_word(source), ct) && is_alpha(target, ct) 
// 	 && swb_trg_stop.find(target) == swb_trg_stop.end()
// 	 && p > threshold)
// 	{
// 	  WORDID wis = s2i_source.getId(source);
// 	  WORDID wit = s2i_target.getId(target);

// 	  mwb[wis].insert(wit);
// 	  target_words1.insert(wit);
// 	}
//     }

//   CLUSTERTABLE clustertable(s2i_target.size(), 0);
//   CLUSTERBAG clusterbag;

//   WORDID lastid = 1;
//   for(MWB::iterator its = mwb.begin(); 
//       its != mwb.end(); its++)
//     {
//       for(WORDBAG::iterator itt1 = (its->second).begin();
// 	  itt1 != (its->second).end();
// 	  itt1++)
// 	{
// 	  for(WORDBAG::iterator itt2 = (its->second).begin();
// 	      itt2 != (its->second).end();
// 	      itt2++)
// 	    {
//  	      string s1 = s2i_target.getStr(*itt1);
//  	      string s2 = s2i_target.getStr(*itt2);

// 	      PROB p_jwd;
// 	      if(s1 == s2)
// 		p_jwd = 3;
// 	      else
// 		p_jwd = jwd(s1, s2);

// 	      if(p_jwd > sthresh1)
// 		{
// 		  PROB p_lcs = 3;

// 		  if(p_jwd < 2)
// 		    p_lcs = lcs_dist(s1, s2);

// 		  if(p_lcs > sthresh2)
// 		    {
// 		      WORDID id = 0, cid1, cid2;

// 		      cid1 = clustertable[*itt1];
// 		      cid2 = clustertable[*itt2];
		      
// 		      if(cid1 != 0 && cid2 != 0)
// 			{
// 			  if(cid1 != cid2)
// 			    {
// 			      WORDBAG t_wb;
// 			      if(join(clusterbag[cid1-1], clusterbag[cid2-1], s2i_target), jthresh)
// 				{
// 				  for(WORDBAG::iterator cbit = clusterbag[cid1-1].begin();
// 				      cbit != clusterbag[cid1-1].end(); cbit++)
// 				    {
// 				      clustertable[*cbit] = cid2;
// 				    }
// 				  clusterbag[cid1-1] = t_wb;
// 				}
// 			      continue;
// 			    }
// 			  else
// 			    {
// 			      continue;
// 			    }
// 			}
// 		      else if(cid1 != 0)
// 			{
// 			  id = cid1;
// 			}
// 		      else if(cid2 != 0)
// 			{
// 			  id = cid2;
// 			}
// 		      else
// 			{
// 			  id = lastid++;
// 			  WORDBAG t_wb;
// 			  clusterbag.push_back(t_wb);
// 			}

// 		      clustertable[*itt1] = clustertable[*itt2] = id;

// 		      clusterbag[id-1].insert(*itt1);
// 		      clusterbag[id-1].insert(*itt2);
// 		    }
// 		}
//  	    }
//  	}
//     }

//   UINT v_idx = 0, n_idx = 0, j_idx = 0, cluster_idx = 0;

//   BEANCOUNTER bc_verb, bc_noun, bc_adjs;

//   string t_fname = fname + ".verb";
//   ofstream of_verb(t_fname.c_str());
//   t_fname = fname + ".noun";
//   ofstream of_noun(t_fname.c_str());
//   t_fname = fname + ".adjs";
//   ofstream of_adjs(t_fname.c_str());
//   t_fname = fname + ".all";
//   ofstream of_all(t_fname.c_str());

//   vector<vector<string> > verb_bag, noun_bag, adjs_bag;
// #ifdef DEBUG
//   cerr << "Entering segmentation" << endl;
// #endif

  
//   for(CLUSTERBAG::iterator it = clusterbag.begin(); it != clusterbag.end(); it++)
//     {
//       vector<string> vs;
//       vector<vector<string> >* tvvs;
//       string out_string;
//       Str2IdMap<WORDID>* ts2i;
//       ofstream* os;
//       EVALBIN* eb;
//       if(it->size())
// 	{
// 	  PROB p_verb = 1, p_noun = 1, p_adj = 1;
// 	  for(WORDBAG::iterator wbit = it->begin(); wbit != it->end(); wbit++)
// 	    {
// 	      p_verb *= pos_dict[*wbit]['V'];
// 	      p_noun *= pos_dict[*wbit]['N'];
// 	      p_adj *= pos_dict[*wbit]['J'];
// 	      vs.push_back(s2i_target.getStr(*wbit));
// 	      out_string += s2i_target.getStr(*wbit) + " ";
// 	    }

// 	  BEANCOUNTER* bcp;	  
// 	  if(p_verb > p_noun)
// 	    {
// 	      if(p_verb > p_adj)
// 		{
// 		  os = &of_verb;
// 		  cluster_idx = ++v_idx;
// 		  bcp = &bc_verb;
// 		  eb = &mybin_verb;
// 		  tvvs = &verb_bag;
// 		  ts2i = &s2i_verb;
// 		}
// 	      else
// 		{
// 		  os = &of_adjs;
// 		  cluster_idx = ++j_idx;
// 		  bcp = &bc_adjs;
// 		  eb = &mybin_adjs;
// 		  tvvs = &adjs_bag;
// 		  ts2i = &s2i_adjs;
// 		}
// 	    }
// 	  else if(p_noun > p_adj)
// 	    {
// 	      os = &of_noun;
// 	      cluster_idx = ++n_idx;
// 	      bcp = &bc_noun;
// 	      tvvs = &noun_bag;
// 	      eb = &mybin_noun;
// 	      ts2i = &s2i_noun;
// 	    }
// 	  else
// 	    {
// 	      os = &of_adjs;
// 	      cluster_idx = ++j_idx;
// 	      bcp = &bc_adjs;
// 	      tvvs = &adjs_bag;
// 	      eb = &mybin_adjs;
// 	      ts2i = &s2i_adjs;
// 	    }

// 	  *os << endl << "#This is cluster #" << cluster_idx << " with the word(s):" << endl
// 	      << "\t";
// 	  *os << out_string << endl;
// 	  of_all << out_string << endl;
// // 	  if(vs.size() > 0)
// 	  tvvs->push_back(vs);
// 	  (*eb)[vs[0]] = new set<string>;
// 	  (*eb)[vs[0]]->insert(vs[0]);
// 	  ts2i->getId(vs[0]);
// 	  if(vs.size() > 1)
// 	    {
//  	      try
//  		{
// 		  Patricia pat(vs[0]);

// 		  for(UINT i = 1; i < vs.size(); i++)
// 		    {
// 		      pat.insert(vs[i]);
// 		      (*eb)[vs[i]] = (*eb)[vs[i-1]];
// 		      (*eb)[vs[i]]->insert(vs[i]);
// 		      ts2i->getId(vs[i]);
// 		    }

// 		  pat.collapse();
// 		  const MA& ma = pat.classify();
// 		  pat.prettyPrint(*os);

// 		  for(MA::const_iterator cit = ma.begin(); cit != ma.end(); cit++)
// 		    {
// 		      for(set<string>::const_iterator csit = (cit->second).begin();
// 			  csit != (cit->second).end(); csit++)
// 			{
// 			  (*bcp)[cit->first][*csit]++;
// 			}
// 		    }
// 		}
// 	      catch (int)
// 		{
// 		  cerr << "Error at: ";
// 		  copy(vs.begin(), vs.end(), ostream_iterator<string>(cerr, " "));
// 		  cerr << endl;
// 		}
// 	    }
// 	}
//     }

// #ifdef DEBUG
//   cerr << "Segmentation finished!" << endl;
// #endif
//   of_verb.close();
//   of_noun.close();
//   of_adjs.close();
//   of_all.close();

// #ifdef DEBUG
//   cerr << "Entering first eval!" << endl;
// #endif
//   t_fname = fname + ".pre.recall";
//   ofstream of_eval(t_fname.c_str());
//   do_cluster_recall(of_eval, celex, mybin_verb);
//   do_cluster_recall(of_eval, celex, mybin_noun);
//   do_cluster_recall(of_eval, celex, mybin_adjs);
//   of_eval.close();
//   t_fname = fname + ".pre.precision";
//   of_eval.open(t_fname.c_str());
//   do_cluster_precision(of_eval, celex, mybin_verb);
//   do_cluster_precision(of_eval, celex, mybin_noun);
//   do_cluster_precision(of_eval, celex, mybin_adjs);
//   of_eval.close();
// #ifdef DEBUG
//   cerr << "Exiting first eval!" << endl;
//   cin.get();
//   cerr << "Deleting bins" << endl;
// #endif

//   delete_evalbin(mybin_verb); delete_evalbin(mybin_noun); delete_evalbin(mybin_adjs);
//   mybin_verb.clear(); mybin_noun.clear(); mybin_adjs.clear();

// #ifdef DEBUG
//   cerr << "Deleted bins!" << endl;
//   cin.get();
// #endif

//   vector<string> topten_pref, topten_suf;

//   vector<string> v_max_pref, v_max_suf;
//   vector<string> verb_pref, verb_suf, adjs_pref, adjs_suf, noun_pref, noun_suf;


//   t_fname = fname + ".affixes.verb";
//   of_verb.open(t_fname.c_str());
//   print_beancounter(of_verb, bc_verb, topten_pref, topten_suf);
//   cutoff_affixes(topten_pref, topten_suf, s2i_verb, v_max_pref, v_max_suf);
//   copy(v_max_pref.begin(), v_max_pref.end(), ostream_iterator<string>(of_verb, " "));
//   of_verb << endl;
//   copy(v_max_suf.begin(), v_max_suf.end(), ostream_iterator<string>(of_verb, " "));
//   of_verb << endl;
//   verb_pref = v_max_pref;
//   verb_suf = v_max_suf;
// #ifdef DEBUG
//   cerr << "entering generation for verbs" << endl;
//   copy(verb_pref.begin(), verb_pref.end(), ostream_iterator<string>(cerr, " "));
//   copy(verb_suf.begin(), verb_suf.end(), ostream_iterator<string>(cerr, " "));
//   copy(v_max_pref.begin(), v_max_pref.end(), ostream_iterator<string>(cerr, " "));
//   copy(v_max_suf.begin(), v_max_suf.end(), ostream_iterator<string>(cerr, " "));
//   cin.get();
// #endif
//   generate_forms(of_verb, mybin_verb, verb_bag, verb_suf, verb_pref);
// #ifdef DEBUG
//   cerr << "exiting generation for verbs" << endl;
//   cin.get();
// #endif
//   topten_pref.clear(); topten_suf.clear(); v_max_pref.clear(); v_max_suf.clear();
//   of_verb.close();

//   t_fname = fname + ".affixes.noun";
//   of_noun.open(t_fname.c_str());
//   print_beancounter(of_noun, bc_noun, topten_pref, topten_suf);
//   cutoff_affixes(topten_pref, topten_suf, s2i_noun, v_max_pref, v_max_suf);
//   copy(v_max_pref.begin(), v_max_pref.end(), ostream_iterator<string>(of_noun, " "));
//   of_noun << endl;
//   copy(v_max_suf.begin(), v_max_suf.end(), ostream_iterator<string>(of_noun, " "));
//   noun_pref = v_max_pref;
//   noun_suf = v_max_suf;
// #ifdef DEBUG
//   cerr << "entering generation for nouns" << endl;
//   copy(noun_pref.begin(), noun_pref.end(), ostream_iterator<string>(cerr, " "));
//   copy(noun_suf.begin(), noun_suf.end(), ostream_iterator<string>(cerr, " "));
//   cin.get();
// #endif
//   generate_forms(of_noun, mybin_noun, noun_bag, noun_suf, noun_pref);
// #ifdef DEBUG
//   cerr << "exiting generation for nouns" << endl;
//   cin.get();
// #endif
//   topten_pref.clear(); topten_suf.clear(); v_max_pref.clear(); v_max_suf.clear();
//   of_noun << endl;
//   of_noun.close();

//   t_fname = fname + ".affixes.adjs";
//   of_adjs.open(t_fname.c_str());
//   print_beancounter(of_adjs, bc_adjs, topten_pref, topten_suf);
//   cutoff_affixes(topten_pref, topten_suf, s2i_adjs, v_max_pref, v_max_suf);
//   copy(v_max_pref.begin(), v_max_pref.end(), ostream_iterator<string>(of_adjs, " "));
//   of_adjs << endl;
//   copy(v_max_suf.begin(), v_max_suf.end(), ostream_iterator<string>(of_adjs, " "));
//   of_adjs << endl; 
//   adjs_pref = v_max_pref;
//   adjs_suf = v_max_suf;
// #ifdef DEBUG
//   cerr << "entering generation for adjs" << endl;
//   copy(v_max_pref.begin(), v_max_pref.end(), ostream_iterator<string>(cerr, " "));
//   copy(v_max_suf.begin(), v_max_suf.end(), ostream_iterator<string>(cerr, " "));
//   copy(adjs_pref.begin(), adjs_pref.end(), ostream_iterator<string>(cerr, " "));
//   copy(adjs_suf.begin(), adjs_suf.end(), ostream_iterator<string>(cerr, " "));
//   cin.get();
// #endif
//   generate_forms(of_adjs, mybin_adjs, adjs_bag, adjs_suf, adjs_pref);
// #ifdef DEBUG
//   cerr << "exiting generation for adjs" << endl;
//   cin.get();
// #endif
//   topten_pref.clear(); topten_suf.clear(); v_max_pref.clear(); v_max_suf.clear();
//   of_adjs.close();

//   t_fname = fname + ".post.recall";
//   of_eval.open(t_fname.c_str());
//   do_cluster_recall(of_eval, celex, mybin_verb);
//   do_cluster_recall(of_eval, celex, mybin_noun);
//   do_cluster_recall(of_eval, celex, mybin_adjs);
//   of_eval.close();
//   t_fname = fname + ".post.precision";
//   of_eval.open(t_fname.c_str());
//   do_cluster_precision(of_eval, celex, mybin_verb);
//   do_cluster_precision(of_eval, celex, mybin_noun);
//   do_cluster_precision(of_eval, celex, mybin_adjs);
//   of_eval.close();

//   st.close();
//   ts.close();
//   if_pos_dict.close();
//   if_trg_stop.close();
//   if_celex.close();

//   return 0;

// }


