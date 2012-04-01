/**
 * @file   cl_baseline.cpp
 * @author Taesun Moon <tsmoon@taesun-moons-macbook-air.local>
 * @date   Sat Mar 22 15:46:08 2008
 * 
 * @brief Baseline clustering using first N% characters shared in
 * common between two words.
 * 
 */


#include <unistd.h>


#include <sstream>
using namespace std;


#include "defs.hpp"
#include "algo.hpp"
#include "infltrait.hpp"
#include "eval.hpp"

double len_common_prefix(const string& s1, const string& s2)
{
  LEN maxlen = Max(s1.size(), s2.size());
  LEN l = 0;
  while(s1[l] == s2[l++]);
  l -= 1;

  return double(l) / maxlen;
}

int main(int argc, char* argv[])
{
  ifstream if_pos_dict, if_celex;

  ofstream ofs;
  
  float similarity_threshold = 0;

  bool do_eval = false,
    latex_output = false,
    generate_output = false;

  int c = 0;
     
  if(argc < 2)
    {
      cerr << "USAGE: -p FILE "
	   << "[Optional arguments]" << endl
	   << "\t-p POS_DICTIONARY" << endl
	   << "Optional:" << endl
	   <<"\t[-c CELEX_FILE]" << endl
	   << "\t[-o OUTPUT]" << endl
	   << "\t\t: If unspecified, output will default to stdout" << endl
	   << "\t[-l]" << endl
	   << "\t\t: If you want latex format output"
	   << " turn this on." << endl
	   << "\t[-i SIMILARITY_THRESHOLD]" << endl;
      
      return 1;
    }

  while ((c = getopt (argc, argv, "p:c:o:i:l")) != -1)
    {
      switch (c)
	{
	case 'c':
	  do_eval = true;
	  if_celex.open(optarg);
	  break;
	case 'o':
	  generate_output = true;
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
	default:
	  abort ();
	}
    }


  set<string> ss;
  string token, pos;
  PROB p;
  UINT count;
  while(if_pos_dict >> token >> pos >> p >> count)
    {
      token = latin1_to_ascii(token);
      if(is_alpha(token))
	{
	  ss.insert(token);
	}
    }


  vector<string> vs;
  copy(ss.begin(), ss.end(), inserter(vs, vs.end()));
  sort(vs.begin(), vs.end());

  vector<WORDIDBAG*> vwordidbag;
  set<WORDIDBAG*> swordidbag;
  Str2IdMap<WORDID> s2i;

  vwordidbag.resize(vs.size(), NULL);
  WORDIDBAG* wb = NULL;
  string root = "XXXXXXXXXXXXXXXX";
  for(UINT i = 0; i < vs.size(); ++i)
    {
      WORDID wid = s2i.getId(vs[i]);
      if(vs[i].size() > 1)
	{
	  if(startswith(vs[i], root))
	    {
	      wb->insert(wid);
	      vwordidbag[wid] = wb;
	    }
	  else
	    {
	      root = vs[i];
	      wb = new WORDIDBAG;
	      swordidbag.insert(wb);
	      wb->insert(wid);
	      vwordidbag[wid] = wb;
	    }
	}
      else
	{
	  wb = new WORDIDBAG;
	  wb->insert(wid);
	  swordidbag.insert(wb);
	  vwordidbag[wid] = wb;
	}	
    }

  if(generate_output)
    {
      for(UINT i = 0; i < vwordidbag.size(); ++i)
	{
	  for(WORDIDBAG::iterator it = vwordidbag[i]->begin();
	      it != vwordidbag[i]->end(); ++it)
	    {
	      cout << s2i.getStr(*it) << " ";
	    }
	  cout << endl;
	}
    }

  Infltrait infltrait;
  Infltrait::POSTrait* pt = new Infltrait::POSTrait(UNKNOWN, &infltrait);
  pt->setClusterSet(swordidbag);
  infltrait.addTrait(UNKNOWN, pt);
  
  if(do_eval)
    {
      EvalCelex ec;
//       ifstream if_celex_to_trait("celex_table");
//       ec.setCelexMap(if_celex_to_trait);
      ec.setCelexStandard(if_celex, s2i);
      ec.evalCluster(infltrait);
      if(latex_output)
	ec.printEvalLatex(cout);
      else
	ec.printEval(cout);
    }
  cout.flush();

  for(set<WORDIDBAG*>::iterator it = swordidbag.begin();
      it != swordidbag.end(); ++it)
    {
      delete *it;
    }

  return 0;
}

  
