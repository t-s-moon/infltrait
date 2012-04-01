/**
 * @file   IO.cpp
 * @author Taesun Moon <tsmoon@wireless-128-62-236-156.public.utexas.edu>
 * @date   Mon Mar  2 16:11:45 2009
 * 
 * @brief  
 * 
 * 
 */

#include <cstdio>
#include <fstream>

#ifndef _MSC_VER
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#endif

#include "io/IO.h"

namespace infl {
  Dispenser::Dispenser(const char c[]) {
    load_dir(c);
  }

  Dispenser::~Dispenser() {
    if(prev_ifstream != NULL)
      delete prev_ifstream;
  }

  const char* Dispenser::load_dir(const char c[]) {
    prev_ifstream = NULL;
    clear();

    if(!chdir(c)) {
    
      getcwd(cwd, BUFFSZ);
      string scwd(cwd); scwd += '/';
      DIR *dp, *subdp;
      dirent *ep;
     
      dp = opendir(cwd);
      if (dp != NULL) {
	while ((ep = readdir (dp))) {
	  if (ep->d_type == DT_REG || ep->d_type == DT_UNKNOWN) {
	    handles.push_back
	      (HANDLE(scwd + ep->d_name,
		      T_FILE));
	  } else if (ep->d_type == DT_DIR) {
	    subdp = opendir(ep->d_name);
	    closedir (subdp);
	  }
	}
	closedir (dp);
	return cwd;
      } else {
	std::cerr << "This has not been implemented yet. Sorry."
		  << std::endl;
	throw 0;
      }
    } else {
      if(std::ifstream(c)) {
	handles.push_back
	  (HANDLE(c, T_FILE));
	return ".";
      } else {
	std::cerr << "This has not been implemented yet. Sorry."
		  << std::endl;
	throw 0;
      }
    }
  }

  size_t Dispenser::size() const {
    return handles.size();
  }
  
  std::istream& Dispenser::operator[](size_t st) {
    switch(handles.at(st).second) {
    case T_FILE:
      if(prev_ifstream != NULL) {
	prev_ifstream->close();
	delete prev_ifstream;
	prev_ifstream = NULL;
      }
      
      prev_ifstream = new std::ifstream(handles.at(st).first.c_str());
      return *prev_ifstream;
    case T_DIR:
    case T_DOC:
      std::cerr << "This has not been implemented yet"
		<< std::endl;
      throw 0;
    }
    throw 0;
  }

  void Dispenser::clear() {
    handles.clear();
    if(prev_ifstream != NULL)
      delete prev_ifstream;
  }
}
