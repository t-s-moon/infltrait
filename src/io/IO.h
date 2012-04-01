/**
 * @file   IO.h
 * @author Taesun Moon <tsmoon@wireless-128-62-236-156.public.utexas.edu>
 * @date   Mon Mar  2 16:10:58 2009
 * 
 * @brief Classes and functions for handling multiple files and
 * documents
 * 
 */

#ifndef __IO_H__
#define __IO_H__

#include <iostream>
#include <string>
#include <utility>
#include <vector>

#include "Defs.h"

namespace infl {

  using std::string;
  
  class Dispenser;
  class dirs {
    friend class Dispenser;
    dirs();
  };
  
  class files {
    friend class Dispenser;
    files();
  };
  
  class docs {
    friend class Dispenser;
    docs();
  };

  class filter {
    
  };

  class Dispenser {
  private:
    enum TYPE {T_DIR, T_FILE, T_DOC}; /**<  */

    typedef std::pair<std::string, TYPE> HANDLE; /**<  */
    typedef std::vector<HANDLE> HANDLES; /**<  */

  private:
    static const unsigned int BUFFSZ = 1024; /**<  */
    char cwd[BUFFSZ];

  private:
    HANDLES handles;		/**<  */

    std::ifstream * prev_ifstream; /**<  */
    
  public:
    Dispenser(const char[] = ".");
    ~Dispenser();
    
    /** 
     * 
     * 
     * @param char 
     */
    const char* load_dir(const char[]);

    /** 
     * 
     * 
     * 
     * @return 
     */
    size_t size() const;

    /** 
     * 
     * 
     * @param size_t 
     * 
     * @return 
     */
    std::istream& operator[](size_t);
    
  private:

    /** 
     * 
     * 
     */
    void clear();

    /** 
     * 
     * 
     */
    void file_handler();
    void dir_handler();
    void doc_handler();
  };
}

#endif
