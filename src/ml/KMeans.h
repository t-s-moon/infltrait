#ifndef __KMEANS_H__
#define __KMEANS_H__

#include <algorithm>
#include <functional>
#include <iostream>
#include <iterator>
#include <map>
#include <string>
#include <vector>

#include <cfloat>
#include <cmath>
#include <cstdio>

#include "ds/LabeledVector.h"
#include "ds/TermByDoc.h"

namespace infl {

  using std::string;
  
  template<typename T>
  class KMeans {
  public:
    template <typename K>
    friend std::ostream& operator<<(std::ostream&,const KMeans<K>&);

    typedef LabelIdxVector<T>* KVECP; /**< Feature vector for
					 calculating kmeans */
    typedef std::map<KVECP, size_t> MTRIPLEVT;

    typedef LabelIdxVector<double>* KVECPD;
    typedef LabelIdxVector<float>* KVECPF;
    typedef std::map<KVECPD, size_t> MTRIPLEVD;
    typedef std::map<KVECPF, size_t> MTRIPLEVF;
    
  private:

    size_t cl;			/**< Number of cluster targets */
    size_t fs;			/**< Vector length/size of features */

    bool verbose,		/**<  */
      cluster_idx_assigned;
    std::vector<std::vector<double>*> centroids;

    MTRIPLEVT features;

  public:
    /** 
     * 
     * 
     * @param n 
     */
    KMeans(int n=2) : cl(n) {
      verbose = true;
      cluster_idx_assigned = false;
#ifndef DEBUG
      srand(time(NULL));
#endif
    }

    /** 
     * 
     * 
     */
    virtual ~KMeans() {
      for(size_t i = 0; i < centroids.size(); ++i)
	delete centroids.at(i);
    }
    
    /** 
     * 
     * 
     * @param k 
     */
    virtual void add_vector(KVECP k) {
      features[k] = rand() % cl;
      print(k);
    }

    template <typename K>
    void cluster(TBD<K>& tbd) {
      seed(tbd);
      cluster();
    }
    
    /** 
     * 
     * 
     */
    virtual void cluster() {
      fs = features.begin()->first->size();
      for(size_t i = 0; i < cl; ++i) {
	centroids.push_back(new std::vector<double>(fs,0));
      }
      while(true) {
	size_t change_count = 0;
	int cluster_sizes[cl];
	for(size_t i = 0; i < cl; ++i)
	  cluster_sizes[i] = 0;

	for(typename MTRIPLEVT::iterator it = features.begin();
	    it != features.end(); ++it) {
	  cluster_sizes[it->second]++;
	  std::transform
	    (it->first->begin(), it->first->end(),
	     centroids[it->second]->begin(), centroids[it->second]->begin(),
	     std::plus<double>());
#ifdef MDEBUG
	  std::cerr << "This is cluster " << it->second << std::endl;
	  print(it->first);
	  print(centroids[it->second]);
#endif
	}
	for(size_t i = 0; i < cl; ++i) {
	  std::transform
	    (centroids[i]->begin(), centroids[i]->end(),
	     centroids[i]->begin(),
	     std::bind2nd
	     (std::divides<double>(), static_cast<double>(cluster_sizes[i])));
	}
	for(typename MTRIPLEVT::iterator it = features.begin();
	    it != features.end(); ++it) {
	  double mindist = DBL_MAX;
	  size_t first_idx = it->second;
	  for(size_t i = 0; i < cl; ++i) {
	    double dist = distance(it->first, centroids[i]);
	    if(mindist > dist) {
	      mindist = dist;
	      it->second = i;
	    }
	  }
	  if(it->second != first_idx)
	    change_count++;
	}
	if (change_count == 0) {
	  assign_cluster_idx();
	  return;
	}
	for(size_t i = 0; i < cl; ++i) {
	  print(centroids[i]);
	  centroids[i]->clear();
	  centroids[i]->assign(fs, 0);
	}
      }
    }

    /** 
     * 
     * 
     * 
     * @return 
     */
    inline const MTRIPLEVT& get_features() {
      return features;
    }

    /** 
     * 
     * 
     * @param tbd 
     */
    template <typename K>
    void seed(TBD<K>& tbd) {
      typename TBD<K>::LIMATRIX& m = tbd.get_matrix();
      for(size_t i = 0; i < m.size(); ++i)
	add_vector(m[i]);
    }

    void print(std::ostream& os) const {
        for(typename MTRIPLEVT::const_iterator it = features.begin();
	    it != features.end(); ++it) {
	  os << it->first->at(0) << " " << it->first->at(1);
	  //       std::cout << it->first->cluster_idx << "\t"
	  // 		<< it->first->label << "\t";
	  //       std::copy(it->first->begin(), it->first->end(),
	  // 		std::ostream_iterator<size_t>(std::cout, ","));
	  os << std::endl;
	}
    }

  protected:
    
    /** 
     * 
     * 
     * @param v1 
     * @param v2 
     * 
     * @return 
     */
    double distance(const std::vector<T>* const v1,
		    const std::vector<double>* const v2) const {
      double sum = 0;
      for(size_t i = 0; i < v1->size(); ++i) {
	sum += pow(v1->at(i) - v2->at(i), 2);
      }
      return sqrt(sum);
    }

    /** 
     * 
     * 
     */
    void assign_cluster_idx() {
      // Workaround to circumvent const-ness of map keys
      if(!cluster_idx_assigned) {
	for(typename MTRIPLEVT::iterator it = features.begin();
	    it != features.end(); ++it) {
	  it->first->cluster_idx = it->second;
	}
      }
      cluster_idx_assigned = true;
    }
    
    /** 
     * 
     * 
     * @param v 
     */
    template<typename K>
    void print(const std::vector<K>* const v) const {
#ifdef MDEBUG
      std::copy(v->begin(), v->end(),
		std::ostream_iterator<K>(std::cerr, ", "));
      std::cerr << std::endl;
#endif
    }
  };

  template<typename T>
  class KMeansSparse : public KMeans<T> {
  public:

    typedef LabelIdxMap* KVECP; /**< Feature vector for
					 calculating kmeans */
    typedef std::map<KVECP, size_t> MTRIPLEVT;

  private:
    size_t cl;			/**< Number of cluster targets */
    size_t fs;			/**< Vector length/size of features */

    std::vector<std::vector<double>*> centroids;
    TermByDocSparse* tbds;

  public:

    /** 
     * 
     * 
     * @param n 
     */
    KMeansSparse(TermByDocSparse* _tbds, int n=2)
      : cl(n), tbds(_tbds) {
#ifndef DEBUG
      srand(time(NULL));
#endif
    }

    KMeansSparse(TermByDocSparse& _tbds, int n=2)
      : cl(n), tbds(&_tbds) {
#ifndef DEBUG
      srand(time(NULL));
#endif
    }
    /** 
     * 
     * 
     */
    ~KMeansSparse() {
      for(size_t i = 0; i < centroids.size(); ++i)
	delete centroids.at(i);
    }

    inline void add_to_centroid(LabelIdxMap* lim) {
      for(LabelIdxMap::iterator it = lim->begin();
	  it != lim->end(); ++it) {
	size_t idx = it->first;
	size_t count = it->second;
	centroids[lim->cluster_idx]->at(idx) += count;
      }
    }
    
    /** 
     * 
     * 
     */
    virtual void cluster() {
      fs = tbds->size();

      for(size_t i = 0; i < cl; ++i)
	centroids.push_back(new std::vector<double>(fs,0));
      for(TermByDocSparse::iterator it = tbds->begin();
	  it != tbds->end(); ++it) 
	it->second->cluster_idx = rand() % cl;

      while(true) {
	size_t change_count = 0;
	int cluster_sizes[cl];
	double cluster_norms[cl];
	for(size_t i = 0; i < cl; ++i)
	  cluster_norms[i] = cluster_sizes[i] = 0;

	for(TermByDocSparse::iterator it = tbds->begin();
	    it != tbds->end(); ++it) {
	  cluster_sizes[it->second->cluster_idx]++;
	  add_to_centroid(it->second);
	}
	for(size_t i = 0; i < cl; ++i) {
	  std::transform
	    (centroids[i]->begin(), centroids[i]->end(),
	     centroids[i]->begin(),
	     std::bind2nd
	     (std::divides<double>(), static_cast<double>(cluster_sizes[i])));
	  for(size_t j = 0; j < centroids[i]->size(); ++j)
	    cluster_norms[i] += pow(centroids[i]->at(j), 2);
	}
	
	for(TermByDocSparse::iterator it = tbds->begin();
	    it != tbds->end(); ++it) {
	  double mindist = DBL_MAX;
	  size_t first_idx = it->second->cluster_idx;
	  for(size_t i = 0; i < cl; ++i) {
	    double dist = distance(it->second, centroids[i], cluster_norms[i]);
	    if(mindist > dist) {
	      mindist = dist;
	      it->second->cluster_idx = i;
	    }
	  }
	  if(it->second->cluster_idx != first_idx)
	    change_count++;
	}
	if (change_count == 0) {
	  return;
	}
	for(size_t i = 0; i < cl; ++i) {
	  centroids[i]->clear();
	  centroids[i]->assign(fs, 0);
	}
      }
    }

    void print(std::ostream& os) const {
        for(TermByDocSparse::const_iterator it = tbds->begin();
	    it != tbds->end(); ++it) {
	  os << it->first << " " << it->second->cluster_idx;
	  //       std::cout << it->first->cluster_idx << "\t"
	  // 		<< it->first->label << "\t";
	  //       std::copy(it->first->begin(), it->first->end(),
	  // 		std::ostream_iterator<size_t>(std::cout, ","));
	  os << std::endl;
	}
    }

  private:
    
    /** 
     * 
     * 
     * @param v1 
     * @param v2 
     * 
     * @return 
     */
    double distance(const LabelIdxMap* const lim,
		    const std::vector<double>* const v2) const {
      double sum = 0;
      for(size_t i = 0; i < v2->size(); ++i)
	sum += pow(v2->at(i), 2);

      for(LabelIdxMap::const_iterator it = lim->begin();
	  it != lim->end(); ++it) {
	double v2_val = v2->at(it->first);
// 	sum -= pow(v2_val, 2);
// 	sum += pow(it->second - v2_val, 2);
	sum += pow(it->second, 2) - 2 * it->second * v2_val;
      }
      return sqrt(sum);
    }

    double distance(const LabelIdxMap* const lim,
		    const std::vector<double>* const v2,
		    double sum) const {
      for(LabelIdxMap::const_iterator it = lim->begin();
	  it != lim->end(); ++it) {
	double v2_val = v2->at(it->first);
	sum += pow(it->second, 2) - 2 * it->second * v2_val;
      }
      return sqrt(sum);
    }
  };

  /** 
   * 
   * 
   * @param os 
   * @param km 
   * 
   * @return 
   */
  template <typename T>
  std::ostream& operator<<(std::ostream& os, const KMeans<T>& km) {
    if(km.verbose)
      os << "Cluster results:" << std::endl;
    for(typename KMeans<T>::KVECL::const_iterator it =
	  km.feature_clusters.begin();
	it != km.feature_clusters.end(); ++it) {
      os << it->second << ": ";
      std::copy(it->first->begin(), it->first->end(),
		     std::ostream_iterator<T>(os, ", "));
      os << std::endl;
    }

    if(km.verbose)
      os << "Cluster means:" << std::endl;
    for(size_t i = 0; i < km.cl; ++i) {
      std::copy(km.centroids[i]->begin(), km.centroids[i]->end(),
		std::ostream_iterator<double>(os, ", "));
      os << std::endl;
    }

    return os;
  }
}

#endif
