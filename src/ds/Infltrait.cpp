#include <cstdlib>

#include "Infltrait.h"

namespace infl {
  
  string traitnames_to_string(TRAITNAMES tn)
  {
    switch(tn) {
    case NOUN:
      return "NN";
    case VERB:
      return "VB";
    case ADJ:
      return "JJ";
    case ADV:
      return "RB";
    default:
      return "";
    }
  }

  char traitnames_to_char(TRAITNAMES tn)
  {
    switch(tn) {
    case NOUN:
      return 'N';
    case VERB:
      return 'V';
    case ADJ:
      return 'J';
    case ADV:
      return 'R';
    default:
      return 0;
    }
  }

  TRAITNAMES string_to_traitnames(string& s)
  {
    char c = s[0];

    switch(c) {
    case 'N':
      return NOUN;
    case 'V':
      return VERB;
    case 'J':
      return ADJ;
    case 'R':
      return ADV;
    default:
      return UNKNOWN;
    }
  }

  //const double Infltrait::POSTrait::SMOOTH_PROB = 1e-14;
  const double Infltrait::POSTrait::SMOOTH_PROB = 1;

  Infltrait::POSTrait::POSTrait(TRAITNAMES tn, const Infltrait* infl)
  {
    pos = tn;
    prob = 1;

    container = infl;

    handle_is_file = false;
    if(container->output_header != "")
      {
	string filename = container->output_header
	  + "."
	  + traitnames_to_string(tn);
	out_handle = new ofstream(filename.c_str());
	handle_is_file = true;
      }
    else
      {
	out_handle = &cout;
      }
  }

  Infltrait::POSTrait::~POSTrait()
  {
    if(handle_is_file)
      delete out_handle;
  }

  void Infltrait::POSTrait::reset_prob()
  {
    prob = 1;
  }

  void Infltrait::POSTrait::set_prob(PROB p)
  {
    if(p != 0)
      prob *= p;
    else
      prob *= SMOOTH_PROB;
  }

  void Infltrait::POSTrait::set_cluster_set(const set<WORDIDBAG*>& sw)
  {
    copy(sw.begin(), sw.end(), inserter(cluster_set, cluster_set.begin()));
  }

  char Infltrait::POSTrait::get_pos() const
  {
    return traitnames_to_char(pos);
  }

  TRAITNAMES Infltrait::POSTrait::get_traitname() const
  {
    return pos;
  }

  const set<WORDIDBAG*>& Infltrait::POSTrait::get_cluster_set() const
  {
    return cluster_set;
  }

  void Infltrait::POSTrait::add_cluster(WORDIDBAG* wb)
  {
    cluster_set.insert(wb);
  }

  PROB Infltrait::POSTrait::get_prob() const
  {
    return prob;
  }

  void Infltrait::POSTrait::set_stream()
  {
    for(set<WORDIDBAG*>::const_iterator it = cluster_set.begin();
	it != cluster_set.end(); ++it)
      {
	for(WORDIDBAG::const_iterator its = (*it)->begin();
	    its != (*it)->end(); ++its)
	  *out_handle << container->s2i_target.getStr(*its) << " ";
	*out_handle << endl;
      }
  }

  void Infltrait::POSTrait::set_stream(ostream& os) const
  {
    for(set<WORDIDBAG*>::const_iterator it = cluster_set.begin();
	it != cluster_set.end(); ++it)
      {
	for(WORDIDBAG::const_iterator its = (*it)->begin();
	    its != (*it)->end(); ++its)
	  os << container->s2i_target.getStr(*its) << " ";
	os << endl;
      }
  }

  const double Infltrait::ALIGNMENT_THRESHOLD = 0.01;
  const double Infltrait::SIMILARITY_THRESHOLD = 0.9;


  Infltrait::Infltrait(double at, double st, string oh)
  {
    clustered = segmented = generated = false;

    if(oh == "")
      {
	output_header = "clusters";
      }
    else
      {
	output_header = oh;
      }
  
    if(at == 0)
      alignment_threshold = ALIGNMENT_THRESHOLD;
    else
      alignment_threshold = at;

    if(st == 0)
      similarity_threshold = SIMILARITY_THRESHOLD;
    else
      similarity_threshold = st;
  
    //   locale loc = locale(locale::classic(), new AnsiCType);
    //   //  locale loc = locale(locale(""), new AnsiCType);
    //   ct = &use_facet<ctype<char> >(loc);
  }



  Infltrait::~Infltrait()
  {
    for(TRAITBIN::iterator it = tb.begin();
	it != tb.end();
	++it)
      {
	delete it->second;
      }

    for(set<WORDIDBAG*>::iterator it = cluster_set.begin();
	it != cluster_set.end(); it++)
      {
	delete *it;
      }
  }


  void Infltrait::add_trait(TRAITNAMES tn, POSTrait* pt)
  {
    if(tb.find(tn) == tb.end())
      {
	if(pt == NULL)
	  {
	    tb[tn] = new POSTrait(tn, this);
	  }
	else
	  {
	    tb[tn] = pt;
	  }
      }
  }


  const Infltrait::POSTrait* Infltrait::get_trait(TRAITNAMES tn)
  {
    return tb[tn];
  }

  const set<WORDIDBAG*>& Infltrait::get_cluster_set() const
  {
    return cluster_set;
  }

  void Infltrait::read_stopwords(istream& is)
  {
    string word;
    while(is >> word)
      {
	stopwords.insert(latin1_to_ascii(word));
      }
  }


  void Infltrait::read_pos_dict(istream& is)
  {
    string token, pos;
    UINT count;
    PROB p;
    is_alpha(token);
    while(is >> token >> pos >> p >> count)
      {
	token = latin1_to_ascii(token);
	if(is_alpha(token) 
	   && stopwords.find(token) == stopwords.end())
	  {
	    pos_dict[s2i_target.getId(token)][pos[0]] += p;
	  }
      }
  }


  void Infltrait::read_source_to_target(istream& is)
  {
    string line, target, source;
    PROB p;
    while(getline(is, line))
      {
	split_line(line, target, source, p);
	target = latin1_to_ascii(target);
	source = latin1_to_ascii(source);
	if(source != "NULL" &&
	   is_alpha(get_word(source)) &&
	   is_alpha(target) &&
	   stopwords.find(target) == stopwords.end() &&
	   p > alignment_threshold)
	  {
	    WORDID wis = s2i_source.getId(source);
	    WORDID wit = s2i_target.getId(target);

	    source_to_target_table[wis].insert(wit);
	  }
      }
  }


  void Infltrait::read_target_to_source(istream& is)
  {
    string line, source, target;
    PROB p;
    while(getline(is, line))
      {
	split_line(line, source, target, p);
	target = latin1_to_ascii(target);
	source = latin1_to_ascii(source);

	if(target != "NULL" &&
	   is_alpha(get_word(source)) &&
	   is_alpha(target) &&
	   stopwords.find(target) == stopwords.end() &&
	   p > alignment_threshold)
	  {
	    WORDID wis = s2i_source.getId(source);
	    WORDID wit = s2i_target.getId(target);

	    source_to_target_table[wis].insert(wit);
	  }
      }

  }

  const Str2IdMap<WORDID>& Infltrait::get_str2id_map() const
  {
    return s2i_target;
  }

  Str2IdMap<WORDID>& Infltrait::get_str2id_map_nc() {
    return s2i_target;
  }

  const Infltrait::TRAITBIN& Infltrait::get_traits() const
  {
    return tb;
  }


  void Infltrait::cluster()
  {
    clusters.resize(s2i_target.size(), NULL);

    for(MWIB::iterator its = source_to_target_table.begin(); 
	its != source_to_target_table.end(); ++its)
      {
	for(WORDIDBAG::iterator itt1 = (its->second).begin();
	    itt1 != (its->second).end();
	    ++itt1)
	  {
	    for(WORDIDBAG::iterator itt2 = (its->second).begin();
		itt2 != (its->second).end();
		++itt2)
	      {
		if(*itt1 != *itt2)
		  {
		    string s1 = s2i_target.getStr(*itt1);
		    string s2 = s2i_target.getStr(*itt2);

		    PROB p_lcs = lcs_dist(s1, s2);

		    if(p_lcs > similarity_threshold)
		      {
			WORDIDBAG* cid1, *cid2;

			cid1 = clusters[*itt1];
			cid2 = clusters[*itt2];
		      
			if(cid1 != NULL && cid2 != NULL)
			  {
			    if(cid1 != cid2)
			      {
				join_nonsingleton(cid1, cid2);
			      }
			    continue;
			  }
			else if(cid1 != NULL)
			  {
			    cid2 = new WORDIDBAG;
			    cid2->insert(*itt2);
			  }
			else if(cid2 != NULL)
			  {
			    cid1 = new WORDIDBAG;
			    cid1->insert(*itt1);
			  }
			else
			  {
			    cid1 = new WORDIDBAG;
			    cid2 = new WORDIDBAG;
			    cid1->insert(*itt1);
			    cid2->insert(*itt2);
			  }

			join(cid1, cid2);
		      }
		  }
	      }
	  }
      }

    for(UINT i = 0; i < s2i_target.size(); ++i)
      {
	if(clusters[i] == NULL)
	  {
	    WORDIDBAG* wib = new WORDIDBAG;
	    wib->insert(i);
	    cluster_set.insert(wib);
	  }
	else
	  {
	    cluster_set.insert(clusters[i]);
	  }
      }
  
    for(set<WORDIDBAG*>::iterator it = cluster_set.begin();
	it != cluster_set.end(); ++it)
      {
	//       if((*it)->size() > 1)
	// 	{
	for(TRAITBIN::iterator tbit = tb.begin();
	    tbit != tb.end(); ++tbit)
	  {
	    tbit->second->reset_prob();
	  }
	for(WORDIDBAG::iterator wbit = (*it)->begin();
	    wbit != (*it)->end(); ++wbit)
	  {
	    for(TRAITBIN::iterator tbit = tb.begin();
		tbit != tb.end(); ++tbit)
	      {
		tbit->second->set_prob
		  (pos_dict[*wbit][tbit->second->get_pos()]);
	      }
	  }

	TRAITNAMES maxpos;
	PROB maxprob = 0;
	for(TRAITBIN::iterator tbit = tb.begin();
	    tbit != tb.end(); ++tbit)
	  {
	    if(tbit->second->get_prob() > maxprob)
	      {
		maxpos = tbit->second->get_traitname();
		maxprob = tbit->second->get_prob();
	      }
	  }
	tb[maxpos]->add_cluster(*it);
      }
    //     }
  
    clustered = true;
  }


  void Infltrait::segment()
  {
    if(!clustered)
      cluster();
    segmented = true;
  }



  void Infltrait::generate()
  {
    if(!segmented)
      segment();
    generated = true;
  }



  void Infltrait::analyze()
  {
    if(!generated)
      generate();
  }


  void Infltrait::print_cluster(ostream& os)
  {
    if(!clustered)
      cluster();
    for(set<WORDIDBAG*>::iterator it = cluster_set.begin();
	it != cluster_set.end(); ++it)
      {
	for(WORDIDBAG::iterator wit = (*it)->begin();
	    wit != (*it)->end(); ++wit)
	  {
	    os << s2i_target.getStr(*wit) << " ";
	  }
	os << endl;
      }
  }

  void Infltrait::print_cluster_by_trait()
  {
    if(!clustered)
      cluster();

    for(TRAITBIN::iterator it = tb.begin(); it != tb.end(); ++it)
      {
	string affix = "." + traitnames_to_string(it->first);
	string filename = output_header + affix;
	ofstream ofs(filename.c_str());
	for(set<WORDIDBAG*>::const_iterator swit =
	      ((it->second)->get_cluster_set()).begin();
	    swit != ((it->second)->get_cluster_set()).end(); ++swit)
	  {
	    for(WORDIDBAG::const_iterator wit = (*swit)->begin();
		wit != (*swit)->end(); ++wit)
	      {
		ofs << s2i_target.getStr(*wit) << " ";
	      }
	    ofs << endl;
	  }
	ofs.close();
      }
  
    string filename = output_header + ".ALL";
    ofstream ofs(filename.c_str());
    print_cluster(ofs);
    ofs.close();
  }

  void Infltrait::print_segment(ostream& os)
  {
    if(!segmented)
      segment();
  }

  void Infltrait::print_generate(ostream& os)
  {
    if(!generated)
      generate();
  }

  void Infltrait::print_analysis(ostream& os)
  {
    print_generate(os);
  }

  void Infltrait::join_nonsingleton(WORDIDBAG* wib1, WORDIDBAG* wib2)
  {
    PROB score = 0;
    vector<PROB> vp;
    for(WORDIDBAG::iterator it = wib1->begin();
	it != wib1->end(); ++it)
      {
	string s1 = s2i_target.getStr(*it);
	for(WORDIDBAG::iterator its = wib2->begin();
	    its != wib2->end(); ++its)
	  {
	    if(*it != *its)
	      {
		string s2 = s2i_target.getStr(*its);
		score = lcs_dist(s1, s2);
		vp.push_back(score);
	      }
	  }
      }

    score = std::accumulate(vp.begin(), vp.end(), 0.0) / vp.size();

    if(score >= similarity_threshold)
      {
	join(wib1, wib2);
      }
  }


  void Infltrait::join(WORDIDBAG* wib1, WORDIDBAG* wib2)
  {
    WORDIDBAG* tid = wib1;
    for(WORDIDBAG::iterator it = wib1->begin();
	it != wib1->end(); ++it)
      {
	wib2->insert(*it);
	clusters[*it] = wib2;
      }
    delete tid;
  }


  void Infltrait::split_line(string& input, string& s1, string& s2, PROB& p)
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

}
