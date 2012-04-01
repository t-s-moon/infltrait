/**
 * @file   defs.hpp
 * @author Taesun Moon <tsmoon@taesun-moons-macbook-air.local>
 * @date   Sat Mar 22 20:37:31 2008
 * 
 * @brief  Global includes and definitions for project
 * 
 * 
 */


#ifndef __DEFS_H__
#define __DEFS_H__

#include <string>

namespace infl{

  typedef unsigned int UINT;
  typedef unsigned short USHORT;
  typedef UINT WORDID;

  typedef double SCORE;
  typedef double PROB;
  typedef std::string::size_type LEN;

#ifndef NULL
  const int NULL = 0;
#endif


  /**
   * Initials stand for:		 
   * 	GS: global search		 
   * 	DS: document delimited search	 
   * 	GC: global clustering		 
   * 	DC: document delimited clustering
   */
  enum EXP_PARAM {
    EXP_PARAM_DEFAULT=2,
    EXP_DS_DC,			/**< this also handles GS_GC
				   experiment settings by dumping one
				   large text file into the data file
				   directory */

    EXP_GS_DC,
    EXP_DS_GC,
    EXP_USP_DS_DC,		/**< From here on down, parameter
				   constants for uspanteko experiments */
    EXP_USP_GS_DC,
    EXP_USP_DS_GC,
    BASELINE_ALL_SINGLETON,	/**< Baseline implementation where
				   all types are dumped into their own
				   cluster */
    BASELINE_ALL_IN_ONE,	/**< Baseline implementation where
				   all types are dumped into a single
				   cluster */
    BASELINE_FIRST_K		/**< Baseline implementation where
				   words which overlap in their first
				   k characters are dumped into the
				   same cluster */
  };

  enum STAT_TEST_PARAM {STAT_TEST_FULL=1,STAT_TEST_DEFAULT};

}
#endif
