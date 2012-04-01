#include <ctime>

#include "Patricia.h"

namespace infl {

  //Static member variable initialization
  UINT Patricia::uid = 0;

  //If l1 is a contiguous subsequence of l2, returns true
  //Only does this by checking whether the first and last elements of l1
  //are in l2.
  template<class T>
  bool pseudo_is_in(const list<T>& l1, const list<T>& l2)
  {
    T first = *(l1.begin()), last = *(l1.rbegin());
    if(find(l2.begin(), l2.end(), first) != l2.end()
       && find(l2.begin(), l2.end(), last) != l2.end())
      return true;
    else 
      return false;
  }

  struct Patricia::Node
  {
    string key;
    set<Node*> from;
    set<Node*> to;

    void getPath(const string&, PATH*, UINT = 0);
    void horizontalCollapse(Patricia*, UINT = 0);
    void print(ostream&, Patricia*, UINT = 0);
    void graphVizPrint(ostream&, Patricia*);
    void classify(Patricia*, UINT = 0);
    bool isInPath(const Node*, Patricia*, UINT = 0);
  };

  class Patricia::ContiguityError : public Error
  {
  private:
    PATH* p1, *p2;

  public:
    ContiguityError(const PATH& pa1, const PATH& pa2) {
      string s1, s2;
      for(PATH::const_iterator it = p1->begin(); it != p1->end(); it++)
	s1 += (*it)->key;
      for(PATH::const_iterator it = p2->begin(); it != p2->end(); it++)
	s2 += (*it)->key;

      message = "Error occurred with sequence \"" + s1
	+ "\" and \"" + s2 + ".\nThe former is not a contiguous"
	"subsequence within the other.";
    }

    const char* m() {
      return message.c_str();
    }
  };

  class Patricia::InfiniteLoopError : public Error
  {
  private:
    const vector<string>* constituents;
    Patricia* pat;
  public:
    InfiniteLoopError(Patricia* pat) {
      pat->collapsed = true;
      pat->graphVizPrint(*error_handle);

      constituents = &(pat->constituents);
      message = "InfiniteLoopError occurred with strings:\n\t";
      for(UINT i = 0; i < constituents->size(); i++)
	{
	  message += constituents->at(i) + " ";
	}
    }
  
    const char* m() {
      return message.c_str();
    }
  };

  Patricia::Patricia(const string& s) {
    init();
    insert(s);
  }

  Patricia::Patricia() {
    init();
  }

  void Patricia::init() {
      generate_error_file=false;
      collapsed = classified = false;
      head = new Node;
      tail = new Node;
      head->key = "START";
      tail->key = "END";
      reg.insert(head); reg.insert(tail);
  }

  Patricia::~Patricia()
  {
    for(set<Node*>::iterator it = reg.begin(); it != reg.end(); it++)
      {
	delete *it;
	//      *it = 0;
      }
  }

  UINT Patricia::eraseCommon(PATH& p1, const PATH& p2)
  {
    UINT offset = 0;
    bool started = false, contiguous = true, momentary_break = false;
    PATH::iterator first, last;
    for(PATH::iterator it = p1.begin(); it != p1.end(); it++)
      {
	if(find(p2.begin(), p2.end(), *it) != p2.end())
	  {
	    if(!started)
	      {
		started = true;
		last = first = it;
		offset++;
	      }
	    else if(contiguous)
	      {
		offset++;
		last = it;
	      }
	    else if(started && momentary_break)
	      {
		contiguous = false;
	      }
	  }
	else
	  {
	    if(started)
	      {
		momentary_break = true;
	      }
	  }
	if(!contiguous)
	  throw ContiguityError(p1, p2);
      }

    if(first != p1.begin())
      offset = 0;

    if(started)
      last++;

    p1.erase(first, last);
    return offset;
  }

  void Patricia::insert(const string& s, UINT thresh)
  {
    PATHBIN path_bin;
    constituents.push_back(s);
    UINT prev_max_bound = 0;
    for(LEN i = 0; i < s.size(); i++)
      {
	PATH* max_path = new PATH;
	UINT max_start = 0;
	vector<PLPAIR> vpath;
	if(charindex.find(s[i]) != charindex.end())
	  {
	    for(NODEPBAG::iterator it = charindex[s[i]].begin(); it != charindex[s[i]].end(); it++)
	      {
		string t = s.substr(i, s.size() - i);
		if(t != "")
		  {
		    PATH* temp_path = new PATH;
		    (*it)->getPath(t, temp_path);
		    if(i + temp_path->size() > prev_max_bound)
		      {
			if(temp_path->size() > max_path->size())
			  {
			    vpath.clear();
			  }
			if(temp_path->size() >= max_path->size())
			  {
			    swap(max_path, temp_path);
			    max_start = i;
			    vpath.push_back(make_pair(*max_path, max_start));
			  }
			prev_max_bound = i + max_path->size();
		      }
		    delete temp_path;
		  }
	      }
	    if(!vpath.empty() && vpath[0].first.size() > thresh)
	      {
		if(!path_bin.empty())
		  {
		    for(UINT j = 0; j < vpath.size(); j++)
		      {
			if(path_bin.size())
			  {
			    Node* first_of = *(vpath[j].first.begin());
			    bool isloop = false;
			    for(PATHBIN::iterator it = path_bin.begin();
				it != path_bin.end(); it++)
			      {
				Node* last = *((it->first).rbegin());
				isloop = first_of->isInPath(last, this, 0);
				if(isloop)
				  {
				    if(pseudo_is_in(it->first, vpath[j].first) && (it->first).size() < vpath[j].first.size())
				      {
					isloop = false;
				      }
				    break;
				  }
			      }
			    if(!isloop)
			      {
				path_bin.insert(path_bin.end(), vpath[j]);
				break;
			      }
			  }
		      }
		  }
		else
		  {
		    path_bin.insert(path_bin.end(), vpath[0]);
		  }
	      }
	  }
	delete max_path;
      }

    //Do two things here. First, 
    //Second, remove overlapping nodes by comparing
    //elements among path_bin. Retain those in larger PATHs while
    //erasing those in smaller PATHs.
    if(path_bin.size())
      {
	PATHBIN::iterator it_stop = path_bin.end(), it_next = path_bin.begin();
	it_stop--; it_next++;
	for(PATHBIN::iterator it1 = path_bin.begin(); it1 != it_stop; it1++, it_next++)
	  {
	    for(PATHBIN::iterator it2 = it_next; it2 != path_bin.end(); it2++)
	      {
		PATH& p1 = it1->first, &p2 = it2->first;
		if(p1.size() && p2.size())
		  {
		    if(p1.size() > p2.size())
		      {
			UINT offset = eraseCommon(p2, p1);
			it2->second += offset;
		      }
		    else
		      {
			UINT offset = eraseCommon(p1, p2);
			it1->second += offset;
		      }
		  }
		else if(p1.size() == 0)
		  {
		    break;
		  }
	      }
	  }

	vector<PATHBIN::iterator> deletion_bin;

	for(PATHBIN::iterator it = path_bin.begin(); it != path_bin.end(); it++)
	  {
	    if((it->first).empty())
	      {
		deletion_bin.push_back(it);
	      }
	  }
      
	for(vector<PATHBIN::iterator>::iterator it = deletion_bin.begin();
	    it != deletion_bin.end(); it++)
	  {
	    path_bin.erase(*it);
	  }

	deletion_bin.clear();
      
	PATHBIN::iterator it_start = path_bin.begin();
	it_start++;
	for(PATHBIN::iterator it1 = it_start; it1 != path_bin.end(); it1++)
	  {
	    for(PATHBIN::iterator it2 = path_bin.begin(); it2 != it1; it2++)
	      {
		Node* source_node = *((it1->first).begin());
		Node* target_node = *((it2->first).rbegin());
		if(source_node->isInPath(target_node, this))
		  {
		    if((it1->first).size() > (it2->first).size())
		      {
			if(find(deletion_bin.begin(), deletion_bin.end(), it1)
			   == deletion_bin.end())
			  deletion_bin.push_back(it1);
		      }
		    else
		      {
			if(find(deletion_bin.begin(), deletion_bin.end(), it2)
			   == deletion_bin.end())
			  deletion_bin.push_back(it2);
		      }
		  }
	      }
	  }

	for(vector<PATHBIN::iterator>::iterator it = deletion_bin.begin();
	    it != deletion_bin.end(); it++)
	  {
	    path_bin.erase(*it);
	  }
      }

    Node* current, *last = head;
    UINT prev_stop = 0, this_start = 0;
    for(PATHBIN::iterator it = path_bin.begin(); it != path_bin.end(); it++)
      {
	if((it->first).size())
	  {
	    this_start = it->second;
	    for(LEN j = prev_stop; j < this_start; j++)
	      {
		current = new Node;
		current->key = s.substr(j,1);
		(current->from).insert(last);
		(last->to).insert(current);

		reg.insert(current);
		charindex[s[j]].insert(current);

		last = current;
	      }
	    if(this_start > prev_stop)
	      {
		((*((it->first).begin()))->from).insert(current);
		(current->to).insert(*((it->first).begin()));
	      }
	    else if(this_start == prev_stop)
	      {
		((*((it->first).begin()))->from).insert(last);
		(last->to).insert(*((it->first).begin()));
	      }
	    else if(this_start < prev_stop)
	      {
		if((it->first).size() > prev_stop - this_start)
		  {
		    UINT t_count = this_start;
		    PATH::iterator itp = (it->first).begin();
		    while(t_count++ < prev_stop)
		      {
			itp++;
		      }
		    ((*itp)->from).insert(last);
		    (last->to).insert(*itp);
		  }
	      }
	    prev_stop = this_start + (it->first).size();
	    last = *((it->first).rbegin());
	  }
      }

    if(prev_stop < s.size())
      {
	for(LEN i = prev_stop; i < s.size(); i++)
	  {
	    current = new Node;
	    current->key = s.substr(i,1);
	    (current->from).insert(last);
	    (last->to).insert(current);
	    reg.insert(current);
	    charindex[s[i]].insert(current);
	    last = current;
	  }
      }
    (last->to).insert(tail);
    (tail->from).insert(last);
  }

  void Patricia::Node::getPath(const string& s, PATH* path, UINT d)
  {
    if(d < s.size() && key[0] == s[d])
      {
	path->insert(path->end(), this);
	for(NODEPBAG::iterator it = to.begin(); it != to.end(); it++)
	  {
	    (*it)->getPath(s, path, ++d);
	  }
      } 
  }

  bool Patricia::findPrintRegNode(Node* n)
  {
    if(print_reg.find(n) != print_reg.end())
      {
	print_reg.erase(n);
	return true;
      }
    else
      return false;
  }

  void Patricia::collapse()
  {
    if(!collapsed)
      {
	UINT d = 0;
	for(set<Node*>::iterator it = (head->to).begin();
	    it != (head->to).end(); it++)
	  {
	    (*it)->horizontalCollapse(this, ++d);
	  }
	collapsed = true;
      }
  }

  void Patricia::Node::horizontalCollapse(Patricia* pat, UINT d)
  {
    if(d > DEPTH_THRESH)
      throw InfiniteLoopError(pat);
    if(key != "START" && key != "END")
      {
	Node* next = *(to.begin());
	while(to.size() == 1 && (next->from).size() == 1 && next->key != "END")
	  {
	    key += next->key;
	    to = next->to;
	    for(set<Node*>::iterator it = to.begin(); it != to.end(); it++)
	      {
		((*it)->from).erase(next);
		((*it)->from).insert(this);
	      }
	    next = *(to.begin());
	  }

	for(set<Node*>::iterator it = to.begin();
	    it != to.end(); it++)
	  {
	    (*it)->horizontalCollapse(pat, ++d);
	  }
      }
  }

  void Patricia::Node::print(ostream& os, Patricia* pat, UINT d)
  {
    if(d > DEPTH_THRESH)
      throw InfiniteLoopError(pat);
  
    if(key != "END")
      {
	if(pat->findPrintRegNode(this))
	  {
	    os << key << "\t";
	  }
	for(set<Node*>::iterator it = from.begin();
	    it != from.end(); it++)
	  {
	    (*it)->print(os, pat, ++d);
	  }
      }
  }

  void Patricia::prettyPrint(ostream& os)
  {
    collapse(); classify();
    print_reg = reg;
    for(set<Node*>::iterator it = (tail->from).begin();
	it != (tail->from).end(); it++)
      {
	os << "\t" << "#These are the segments obtained through the FSM:" << endl
	   << "\t\t";
	(*it)->print(os, this);
	os << endl;
      }

    os << "\t" << "#These are the categorized segments:" << endl
       << "\t\t";

    for(MA::iterator it = morphs.begin(); it != morphs.end(); it++)
      {
	os << morph_name(it->first) << ": ";
	copy((it->second).begin(), (it->second).end(), std::ostream_iterator<string>(os, " "));
	os << endl << "\t\t";
      }
  }

  void Patricia::graphVizPrint(ostream& os)
  {
    collapse();
    uid++;
    print_reg = reg;

    os << "digraph " << uid << " {" << endl;
    os << "\trankdir = LR;" << endl;
    head->graphVizPrint(os, this);
    os << "slartibartfast[shape=plaintext,style=filled,"
      "color=lightgrey,label=\"";
    for(UINT i = 0; i < constituents.size()-1; i++)
      {
	os << constituents[i] << "\\n";
      }
    os << *(constituents.rbegin()) << "\"];" << endl;
    os << "}" << endl;
  }

  void Patricia::Node::graphVizPrint(ostream& os, Patricia* pat)
  {
    if(pat->findPrintRegNode(this))
      {
	os << "\t" << (size_t) this
	   << "[label=" << key << "];" << endl;
	for(set<Node*>::iterator it = to.begin();
	    it != to.end(); it++)
	  {
	    (*it)->graphVizPrint(os, pat);
	    os << "\t" << (size_t) this << " -> "
	       << (size_t) *it << ";" << endl;
	  }
      }
  }

  const MA& Patricia::classify()
  {
    if(!classified)
      {
	for(set<Node*>::iterator it = (head->to).begin(); it != (head->to).end(); it++)
	  {
	    (*it)->classify(this);
	  }
      }
    classified = true;
    return morphs;
  }

  void Patricia::Node::classify(Patricia* pat, UINT d)
  {
    if(d > DEPTH_THRESH)
      throw InfiniteLoopError(pat);

    bool issuffix = false, isprefix = false, isstem = false;
  
    for(set<Node*>::iterator it = to.begin(); it != to.end(); it++)
      {
	if((*it)->key == "END")
	  issuffix = true;
	break;
      }

    for(set<Node*>::iterator it = from.begin(); it != from.end(); it++)
      {
	if((*it)->key == "HEAD")
	  isprefix = true;
	break;
      }

    if((issuffix && to.size() > 1) || (isprefix && from.size() > 1) || (from.size() > 1 && to.size() > 1))
      isstem = true;

    if(issuffix && isprefix)
      {
	pat->addMorph(key, STEM);
      }
    else
      {
	if(issuffix)
	  pat->addMorph(key, SUFFIX);

	if(isprefix)
	  pat->addMorph(key, PREFIX);

	if(isstem)
	  pat->addMorph(key, STEM);
      }

    for(set<Node*>::iterator it = to.begin(); it != to.end(); it++)
      (*it)->classify(pat, ++d);
  }

  bool Patricia::Node::isInPath(const Node* n, Patricia* pat, UINT d)
  {
    if(d > DEPTH_THRESH)
      throw InfiniteLoopError(pat);
    bool found = false;
    if(n == this)
      return true;
    else
      {
	for(NODEPBAG::iterator it = (this->to).begin(); it != (this->to).end(); it++)
	  {
	    found = (*it)->isInPath(n, pat, ++d);
	    if(found)
	      {
		return true;
	      }
	  }
      }
    return false;
  }

  inline void Patricia::addMorph(const string& s, MORPHTYPE m)
  {
    morphs[m].insert(s);
  }

  void Patricia::set_error_log() {
    error_handle = new std::ofstream("error.dot");
  }
  
  void Patricia::close()
  {
    error_handle->close();
    delete error_handle;
  }

}
