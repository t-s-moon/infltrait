//////////////////////////////////////////////////////////////////////////////
// Implementation of a acyclic finite state machine. Insertion		    //
// algorithm is implemented to favor longer contiguous substrings as	    //
// insertion points. It will also ignore single length substring	    //
// matches as insertion points. This, however, can be modified.		    //
//////////////////////////////////////////////////////////////////////////////

#ifndef __PATRICIA_H__
#define __PATRICIA_H__

#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <list>
#include <map>
#include <set>
#include <string>
#include <vector>
#include <utility>

#include "Defs.h"
#include "io/ContainerAlgo.h"
#include "io/StringAlgo.h"

namespace infl {

  using std::endl;
  using std::istream;
  using std::list;
  using std::ostream;
  using std::map;
  using std::pair;
  using std::set;
  using std::string;
  using std::vector;
  
  enum MORPHTYPE {SUFFIX, PREFIX, INFIX, STEM};

  //For generating string output to MORPHTYPE enum types
  inline string morph_name(MORPHTYPE m)
  {
    switch(m)
      {
      case STEM:
	return "STEM";
      case PREFIX:
	return "PREFIX";
      case SUFFIX:
	return "SUFFIX";
      case INFIX:
	return "INFIX";
      default:
	return "UNKNOWN";
      }
  }

  //typedefs for output from Patricia::classify(). Use MA as const
  //container reference to Patricia::classify() output. Use BEANCOUNTER
  //to count instances of particular (MORPHTYPE, type) pair.
  typedef map<MORPHTYPE, map<string, UINT> > BEANCOUNTER;
  typedef map<MORPHTYPE, set<string> > MA;


  //General exception handler to Patricia.
  class Error
  {
  protected:
    string message;
  public:
    virtual ~Error(){}
    virtual const char* m() = 0;
  };


  //
  class Patricia
  {
  private:

    struct Node;
  
    typedef set<Node*> NODEPBAG;
    typedef list<Node*> PATH;
    typedef map<char, NODEPBAG> CHARINDEX;

    typedef pair<PATH, LEN> PLPAIR;
    typedef list<PLPAIR> PATHBIN;

  private:
    std::vector<string> constituents;

    Node* head;
    Node* tail;
    bool classified, collapsed;

    //Registry for all allocated Node instances.
    NODEPBAG reg;
  
    //Temporary registry for file/stream output.
    NODEPBAG print_reg;

    //Keeps track of all nodes given a certain character.
    CHARINDEX charindex;
    MA morphs;

    //This value determines the minimum length of an overlapping subsequence
    const static UINT MIN = 1;

    //This value determines the maximum depth for a recursive procedure
    //before throwing an InifiniteLoopError
    const static UINT DEPTH_THRESH = 100;

    //Unique identifier for digraph structures in graphviz format output
    static UINT uid;

  private:
    static std::ofstream* error_handle; /**<  */
  public:
    bool generate_error_file;	/**<  */

  private:
    inline void addMorph(const string&, MORPHTYPE);
    inline bool findPrintRegNode(Node*);

    void init();
    void collapse();
    UINT eraseCommon(PATH&, const PATH&);

  public:
    class ContiguityError;
    class InfiniteLoopError;

    Patricia(const string&);
    Patricia();
    ~Patricia();

    //Insert a word into the FSM. Build it based on minimum overlap
    //threshold.
    void insert(const string&, UINT = MIN);

    //Print the nodes in the FSM, then the nodes categorized by
    //affix type.
    void prettyPrint(ostream&);

    //Prints the nodes in graphviz dot format. Need to run the program
    //"dot" on the output.
    //Future work: direct to postscript output.
    void graphVizPrint(ostream&);

    void set_error_log();
    //Imperative that this function is called before the
    //termination of main. Closes error_handle and deletes memory.
    void close();

    //Classifies all nodes in patricia given a certain heuristic as to
    //whether it is a STEM, PREFIX, SUFFIX, or INFIX. Returns a constant
    //reference to "morphs" variable.
    const MA& classify();
  };

  std::ofstream* Patricia::error_handle;
}
#endif
