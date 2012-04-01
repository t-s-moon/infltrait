/**
 * @file   cl.cpp
 * @author Taesun Moon <tsmoon@taesun-moons-macbook-air.local>
 * @date   Sat Mar 22 15:46:08 2008
 * 
 * @brief Cluster words from aligned corpora based on inflectional
 * morphology.
 * 
 */


#include <unistd.h>


#include <sstream>
using namespace std;


#include "defs.hpp"
#include "algo.hpp"
#include "infltrait.hpp"
#include "eval.hpp"


int main(int argc, char* argv[])
{
  ifstream if_source_to_target, if_target_to_source,
    if_pos_dict, if_trg_stop, if_celex;

  ofstream ofs;
  
  string output_header;

  float alignment_threshold = 0,
    similarity_threshold = 0;

  bool do_eval = false,
    latex_output = false;

  int c = 0;
     
  if(argc < 2)
    {
      cerr << "USAGE: -t FILE -s FILE -p FILE "
	   << "[Optional arguments] [Non-option arguments]" << endl
	   << "\t-t TARGET_TO_SOURCE" << endl
	   << "\t-s SOURCE_TO_TARGET" << endl
	   << "\t-p POS_DICTIONARY" << endl
	   << "Optional:" << endl
	   <<"\t[-c CELEX_FILE]" << endl
	   << "\t[-w STOPWORDS]" << endl
	   << "\t[-h OUTPUT_HEADER]" << endl
	   << "\t\t: Use this option if you want more detailed output." << endl
	   << "\t[-o OUTPUT]" << endl
	   << "\t\t: If unspecified, output will default to stdout" << endl
	   << "\t\t  This will only output figures." << endl
	   << "\t[-l]" << endl
	   << "\t\t: If you want latex format output"
	   << " turn this on." << endl
	   << "\t[-a ALIGNMENT_THRESHOLD]" << endl
	   << "\t[-i SIMILARITY_THRESHOLD]" << endl
	   << "Non-option arguments:" << endl
	   << "\t[Penn Treebank style POS tags]" << endl
	   << "\t\t: Target POS to be clustered. "
	   << "Any parameters at the end of the command line " << endl
	   << "\t\tthat do not belong to an option will be parsed "
	   << "as POS to be clustered" << endl;
      
      return 1;
    }

  while ((c = getopt (argc, argv, "t:s:p:o:w:a:i:r:c:h:l")) != -1)
    {
      switch (c)
	{
	case 't':
	  if_target_to_source.open(optarg);
	  if(!if_target_to_source)
	    {
	      cerr << optarg << " is not a valid Target"
		" to Source File" << endl;
	      return 1;
	    }
	  break;
	case 's':
	  if_source_to_target.open(optarg);
	  if(!if_source_to_target)
	    {
	      cerr << optarg << " is not a valid Source"
		" to Target File" << endl;
	      return 1;
	    }
	  break;
	case 'w':
	  if_trg_stop.open(optarg);
	  if(!if_trg_stop)
	    {
	      cerr << optarg << " is not a valid target"
		" stopword file" << endl;
	      return 1;
	    }
	  break;
	case 'c':
	  do_eval = true;
	  if_celex.open(optarg);
	  if(!if_celex)
	    {
	      cerr << optarg << " is not a valid Celex"
		" file" << endl;
	      return 1;
	    }
	  break;
	case 'h':
	  output_header = optarg;
	  break;
	case 'o':
	  ofs.open(optarg);
	  cout.rdbuf(ofs.rdbuf());
	  break;
	case 'l':
	  latex_output = true;
	  break;
	case 'p':
	  if_pos_dict.open(optarg);
	  if(!if_pos_dict)
	    {
	      cerr << optarg << " is not a valid pos dictionary"
		   << endl;
	      return 1;
	    }
	  break;
	case 'i':
	  similarity_threshold = atof(optarg);
	  break;
	case 'a':
	  alignment_threshold = atof(optarg);
	  break;
	default:
	  abort ();
	}
    }

  
  Infltrait infltrait(alignment_threshold,
		      similarity_threshold,
		      output_header);		      

  infltrait.readTargetToSource(if_target_to_source);
  infltrait.readSourceToTarget(if_source_to_target);
  infltrait.readStopWords(if_trg_stop);
  infltrait.readPOSDict(if_pos_dict);
  
  if(argc == optind)
    {
      infltrait.addTrait(NOUN);
      infltrait.addTrait(ADJ);
      infltrait.addTrait(VERB);
    }
  else
    {
      for (int index = optind; index < argc; ++index)
	{
	  string tname(argv[index]);
	  infltrait.addTrait(string_to_traitnames(tname));
	}
    }

  infltrait.cluster();
  infltrait.printClusterByTrait();

  if(do_eval)
    {
      EvalCelex ec;
//       ifstream if_celex_to_trait("celex_table");
//       ec.setCelexMap(if_celex_to_trait);
      ec.setCelexStandard(if_celex, infltrait.getStr2IdMap());
      ec.evalCluster(infltrait);
      if(latex_output)
	ec.printEvalLatex(cout);
      else
	ec.printEval(cout, true);
    }
  cout.flush();
  
  return 0;
}

  
//   MWB mwb;
//   MWC mwc_clusters;

//   string line, target, source;
//   Str2IdMap<WORDID> s2i_source, s2i_target, s2i_verb, s2i_noun, s2i_adjs;
//   WORDBAG target_words1, target_words2;
//   PROB p;


// #ifdef DEBUG
//    cout << "CELEX entered!" << endl;
// //   cin.get();
// #endif



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


