/**
 * @file   infltrait.hpp
 * @author Taesun Moon <tsmoon@taesun-moons-macbook-air.local>
 * @date   Sat Mar 22 16:06:23 2008
 * 
 * @brief  Defines the class Infltrait and handles various morphology
 * related tasks: clustering, segmentation, and generation
 * 
 */


#ifndef __INFLTRAIT_H__
#define __INFLTRAIT_H__

#include <fstream>
#include <iostream>
#include <map>
#include <numeric>
#include <set>
#include <string>
#include <vector>

#include "Defs.h"

#include "ds/Str2IdMap.h"

#include "io/StringAlgo.h"
#include "io/AnsiCType.h"
#include "io/Latin1ToAscii.h"

#include "sd/LCS.h"

namespace infl {

  using std::cerr;
  using std::cout;
  using std::endl;
  using std::istream;
  using std::map;
  using std::ofstream;
  using std::ostream;
  using std::set;
  using std::string;
  using std::vector;
  
  typedef set<WORDID> WORDIDBAG;


  /** 
   * @enum TRAITNAMES
   * @brief Constants defining POS traits. Insert new trait you want to
   * use here.
   */
  enum TRAITNAMES
    {
      UNKNOWN,			/**< Default case */
      NOUN,			/**< Nouns */
      VERB,			/**< Verbs  */
      ADJ,			/**< Adjectives */
      ADV,			/**< Adverbs */
    };

  string traitnames_to_string(TRAITNAMES);
  char traitnames_to_char(TRAITNAMES);
  TRAITNAMES string_to_traitnames(string&);


  /** 
   * @class Infltrait
   * 
   * @brief Super container for clusters for given POS tags. Actual
   * container labels are given by enum TRAITNAMES
   * 
   */
  class Infltrait
  {
  public:
    /** 
     * @class POSTrait
     *
     * @brief Container of data for clustering
     */
    class POSTrait
    {
    private:
      friend class Infltrait;
    
    private:
      typedef set<WORDID> WORDIDBAG;

    private:
      TRAITNAMES pos;
      PROB prob;
      const static double SMOOTH_PROB;
      set<WORDIDBAG*> cluster_set;

      const Infltrait* container;

      bool handle_is_file;
      ostream* out_handle;

    public:
      POSTrait(TRAITNAMES, const Infltrait*);
      ~POSTrait();

      void add_cluster(WORDIDBAG*);

      void reset_prob();
      void set_prob(PROB);
      PROB get_prob() const;
      char get_pos() const;
      void set_cluster_set(const set<WORDIDBAG*>&);
      void set_stream();
      void set_stream(ostream& os) const;

      const set<WORDIDBAG*>& get_cluster_set() const;
    
      TRAITNAMES get_traitname() const;
    };

  private:
    typedef set<string> WORDBAG;

    typedef map<char, PROB> WORDPROB;
    typedef map<WORDID, WORDPROB> MWP;
    typedef map<WORDID, WORDIDBAG> MWIB;

    typedef vector<WORDIDBAG*> VWIB;

  public:
    typedef map<TRAITNAMES, POSTrait*> TRAITBIN;
  
  private:  

    /** 
     * 
     * 
     */
    TRAITBIN tb;

    /** 
     * 
     * 
     */
    Str2IdMap<WORDID> s2i_source, s2i_target;

    /** 
     * 
     * 
     */
    WORDBAG stopwords;

    /** 
     * Of type map<unsigned short, map<char, double> >. Maps from the
     * index of a word in the vocabulary to a probability table of part
     * of speech tags, i.e. a mapping for P(pos tag | word).
     */
    MWP pos_dict;

    /** 
     * Of type map<unsigned short, set<unsigned short> >. Alignment
     * dictionary of words from the source to the target. Only
     * alignments which pass a certain threshold are included.
     */
    MWIB source_to_target_table;

    /** 
     * Of type vector< set<unsigned short>*> and is of the size of the
     * vocabulary of the target text.
     */
    VWIB clusters;

    /** 
     * A register of clusters that have been formed by alignment/LCS
     * similarity threshold
     */
    set<WORDIDBAG*> cluster_set;

  private:
    bool clustered, segmented, generated;

    string output_header;

    const ctype<char>* ct;
  
    const static double ALIGNMENT_THRESHOLD;
    const static double SIMILARITY_THRESHOLD;
    const static double SMOOTH_PROB;

    double alignment_threshold;
    double similarity_threshold;

  
  private:
    void split_line(string&, string&, string&, PROB&);
    void join_nonsingleton(WORDIDBAG*, WORDIDBAG*);
    void join(WORDIDBAG*, WORDIDBAG*);

    /** 
     * @brief Prototype for generic programming
     * 
     * @param s1 
     * @param s2 
     */
    template<typename T, class Joiner>
    void joiner(const set<T>& s1, const set<T>& s2)
    {
      typename Joiner::join(s1, s2);
    }
  
  public:
    Infltrait(double = ALIGNMENT_THRESHOLD,
	      double = SIMILARITY_THRESHOLD,
	      string = "");
    ~Infltrait();

    void add_trait(TRAITNAMES, POSTrait* = NULL);
    const POSTrait* get_trait(TRAITNAMES);

    void read_stopwords(istream&);
    void read_pos_dict(istream&);

    /** 
     * @brief Read from target to source alignment file
     * 
     * @param is istream instance
     * 
     * If generated by GIZA++, the alignment file will have a suffix of
     * .actual.ti.final. A line of input will be in the following format:
     *
     *          target_word source_word[|pos] alignment_probability
     *
     * The source_word column may or may not have a pos label separated
     * by a pipe symbol.
     * 
     */
    void read_target_to_source(istream& is);

    /** 
     * @brief Read from source to target alignment file
     * 
     * @param is istream instance
     *
     * If generated by GIZA++, the alignment file will have a suffix of
     * .actual.ti.final. A line of input will be in the following format:
     *
     *          source_word[|pos] target_word alignment_probability
     *
     * The source_word column may or may not have a pos label separated
     * by a pipe symbol.
     * 
     */
    void read_source_to_target(istream& is);

    const Str2IdMap<WORDID>& get_str2id_map() const;
    Str2IdMap<WORDID>& get_str2id_map_nc();
    const TRAITBIN& get_traits() const;

    const set<WORDIDBAG*>& get_cluster_set() const;
  
    void cluster();
    void segment();
    void generate();

    void print_cluster(ostream&);
    void print_cluster_by_trait();
    void print_segment(ostream&);
    void print_generate(ostream&);

    void analyze();
    void print_analysis(ostream&);

  };

}

#endif