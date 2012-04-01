#ifndef __STR2ID_MAP_H__
#define __STR2ID_MAP_H__

#include <map>
#include <iostream>
#include <string>
#include <vector>

namespace infl {

  using std::string;
  
  /**
   * 
   * 
   */
  template <typename T>
  class Str2IdMap {
  private:
    std::map<string, T> _toId; /**< Map to string IDs */
    std::vector<string> _toStr; /**< Map back from IDs to
					strings */

  public:
    
    std::string getStr(T id) const {
      return _toStr[id];
    }

    const std::vector<string>& vStr() const {
      return _toStr;
    }
  
    T getId(const string& str) {
      typename std::map<string, T>::iterator f = _toId.find(str);
      T id;
      if (f==_toId.end()) {
	id = _toId.size();
	_toId[str] = id;
	_toStr.push_back(str);
	return id;
      }
      else 
	return f->second;
    }

    T getCId(const string& str) const {
      typename std::map<string, T>::const_iterator f = _toId.find(str);
      return f->second;
    }

    bool isMember(const string& str) const {
      if(_toId.find(str) == _toId.end())
	return false;
      else
	return true;
    }

    size_t size() const {
      return _toStr.size();
    }

    void print(std::ostream& os) const {
      std::vector<string>::const_iterator it;
      for(it = _toStr.begin(); it != _toStr.end(); it++)
	os << *it << "\t" << _toId[*it] << std::endl;
    }
  };
}

#endif
