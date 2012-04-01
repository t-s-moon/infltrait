/**
 * @file   Driver.cpp
 * @author Taesun Moon <tsmoon@tsmoon-macbook.local>
 * @date   Sat Apr 18 13:44:19 2009
 * 
 * @brief  
 * 
 * 
 */

#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

using std::cerr;
using std::cout;
using std::endl;
using std::string;

#include <cerrno>
#include <unistd.h>
#include <sys/stat.h>

#include "ds/RuleByDoc.h"
#include "ds/RuleByDocPS.h"

#include "io/Baseline.h"

#include "Eval.h"


const size_t BUFFSZ = 2048;

/** 
 * Create directory @p d with default read/write permissions
 * 
 * @param d 
 * 
 * @return 
 */
int mkdir(const char* d) {
  const mode_t PERMISSIONS = S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH;
  return mkdir(d,PERMISSIONS);
}


/** 
 * Write the command line parameters into @p os 
 * 
 * @param argc 
 * @param argv 
 * @param os 
 */

void write_params(int argc, char* argv[], std::ostream& os) {
  int i = 0;
  while(i < argc) {
    os << argv[i++] << " ";
  }
  os << endl;
}

int main(int argc, char* argv[]) {
  
  if(argc < 2) {
    cerr << "USAGE:\t" << argv[0] << endl
	 << "\t-d RAW_DATA:\tlocation of training data. Should be a directory" << endl
	 << "\t-o DESTINATION:\tlocation of output. Should be a directory" << endl
	 << "\t-s STANDARD:\tpath of gold standard." << endl
	 << "\t-b HUMONGO:\tpath of concat of all training files." << endl
	 << "\t-f STRING_FILTER:\tLanguage filter for lowercasing and whatnot." << endl
	 << "\t\t\t0=ENGLISH (default), 1=GERMAN, 2=USPANTEKO, 3=PREFIX, 4=NONE" << endl
	 << "\t-t CONFIDENCE:\tstatistical confidence threshold" << endl
	 << "\t\t\t0=95%, 1=99%, 2=99.8%" << endl
	 << "\t-l BASELINE:\tRun baseline" << endl
	 << "\t\t\t1=all singleton, 2=all-in-one" << endl
	 << "\t-g\tOptional flag if you want doc search and global clustering" << endl
	 << "\t-p\tOptional flag if you want clustering for a language" << endl
	 << "\t\tthat is both prefixal and suffixal" << endl
	 << "\t-e\tOptional flag if you want evaluation with a non-celex format" << endl
	 << "\t-k K-THRESH\tBaseline implementation that clusters"
	 << "words based on shared first k characters" << endl
	 << endl;
    return 1;
  }

  enum baseline_params {all_singleton=1,all_in_one=2};

  string output_path, big_file, input_dir, gold_file;
  char cwd[BUFFSZ];
  getcwd(cwd, BUFFSZ);
  strcat(cwd, "/");
  string current_directory = cwd;
  int c = 0;
  infl::EXP_PARAM exp_param = infl::EXP_DS_DC;
  infl::FILTER_TYPE filter_type=infl::ENGLISH_FILTER;

  size_t confidence_threshold=0;
  size_t min_cell_size = 5;
  size_t k_thresh = 0;
  bool prefix_and_suffix = false, non_celex = false, reverse = false;
  while ((c = getopt (argc, argv, "d:b:o:gc:f:t:l:m:k:pe")) != -1) {
    switch (c) {
    case 'd': // training directory
      input_dir = current_directory + optarg;
      break;
    case 'b': // humongo file with all docs
      exp_param = infl::EXP_GS_DC;
      big_file = current_directory + optarg;
      break;
    case 'm':
      min_cell_size = atoi(optarg);
      break;
    case 'o': // path of output_files, should be directory
      output_path = current_directory + optarg;
      mkdir(output_path.c_str());
      if(ENOENT == errno) {
	cerr << "Could not create directory: " << output_path << endl;
	return 1;
      }
      break;
    case 'c': // path of celex eval file
      gold_file = current_directory + optarg;
      break;
    case 'f': //filter type
      {
	int _ft = atoi(optarg);
	switch(_ft) {
	case 0: filter_type = infl::ENGLISH_FILTER; break;
	case 1: filter_type = infl::GERMAN_FILTER; break;
	case 2: filter_type = infl::USP_FILTER; break;
	case 3: filter_type = infl::REVERSE_FILTER; break;
	case 4: filter_type = infl::NO_FILTER; break;
	default: filter_type = infl::ENGLISH_FILTER;
	}
      }
      break;
    case 't': //confidence threshold
      confidence_threshold = atoi(optarg);
      break;
    case 'l': //baseline type
      {
	int _bt = atoi(optarg);
	if(_bt == all_singleton) {
	  exp_param = infl::BASELINE_ALL_SINGLETON;
	} else if(_bt == all_in_one) {
	  exp_param = infl::BASELINE_ALL_IN_ONE;
	} else {
	  cerr << "Baseline experiment type " << optarg 
	       << " does not exist" << endl;
	  return 1;
	}
      }
      break;
    case 'g':
      exp_param = infl::EXP_DS_GC;
      break;
    case 'k':
      exp_param = infl::BASELINE_FIRST_K;
      k_thresh = atoi(optarg);
      break;
    case 'p':
      prefix_and_suffix = true;
      break;
    case 'e':
      non_celex = true;
      break;
    default:
      cerr << "Huh?" << endl;
      abort();
    }
  }

  string cluster_path, debug_dump_path;
  if(*output_path.rbegin() == '/') {
    cluster_path = output_path + "clusters";
    debug_dump_path = output_path + "debug_dump";
  } else {
    cluster_path = output_path + "/clusters";
    debug_dump_path = output_path + "/debug_dump";
  }

  infl::StringFilter* sf;
  switch(filter_type) {
  case infl::ENGLISH_FILTER:
    sf = new infl::EnglishFilter;
    break;
  case infl::GERMAN_FILTER:
    sf = new infl::GermanFilter;
    break;
  case infl::USP_FILTER:
//     switch(exp_param) {
//     case infl::EXP_DS_GC:
//       exp_param = infl::EXP_USP_DS_GC; break;
//     case infl::EXP_GS_DC:
//       exp_param = infl::EXP_USP_GS_DC; break;
//     default:
//       exp_param = infl::EXP_USP_DS_DC; break;
//     }
//     sf = new infl::UspFilter;
    break;
  case infl::REVERSE_FILTER:
    reverse = true;
    sf = new infl::ReverseUspFilter;
    break;
  case infl::NO_FILTER:
    sf = new infl::StringFilter;
    break;
  default:
    sf = new infl::EnglishFilter;
  }
  
  if(prefix_and_suffix) {
    switch(exp_param) {
    case infl::EXP_DS_GC:
      exp_param = infl::EXP_USP_DS_GC; break;
    case infl::EXP_GS_DC:
      exp_param = infl::EXP_USP_GS_DC; break;
    default:
      exp_param = infl::EXP_USP_DS_DC; break;
    }
    sf = new infl::UspFilter;
  }

  std::ifstream if_celex(gold_file.c_str());
  std::ofstream of_eval(cluster_path.c_str());
  std::ofstream of_dbg_dump(debug_dump_path.c_str());

  infl::EvalCelexStream* ec;
  if(non_celex) {
    ec = new infl::EvalClusterPerLine;
  } else {
    ec = new infl::EvalCelexStream;
  }
  if(if_celex.is_open()) {
    ec->ecos = &of_eval;
  }
  
  std::ostringstream oss;
  
  switch(exp_param) {
  case infl::EXP_DS_DC:
    {
      infl::RBD_DS_DC rbd(sf, input_dir.c_str());
      rbd.ep.confidence = confidence_threshold;
      rbd.ep.min_cell_size = min_cell_size;
      rbd.process_rule_by_doc();
      if(reverse)
	rbd.sc->flip();
      if(if_celex.is_open()) {
	ec->set_standard(if_celex, rbd.get_vocabulary());
	rbd.write_stream(oss);
	write_params(argc, argv, of_dbg_dump);
	std::istringstream is(oss.str());
	ec->eval_all(is);
	rbd.write_debug_stream(of_dbg_dump);
      } else {
	rbd.write_stream(of_eval);
	write_params(argc, argv, of_dbg_dump);
	rbd.write_debug_stream(of_dbg_dump);
      }
    }
    break;
  case infl::EXP_GS_DC:
    {
      infl::RBD_DS_DC rbd(sf, big_file.c_str());
      rbd.ep.confidence = confidence_threshold;
      rbd.ep.min_cell_size = min_cell_size;
      rbd.process_rule_by_doc();
      infl::RBD_GS_DC rbd4(sf, input_dir.c_str());
      rbd4.process_rule_by_doc();
      rbd.sc->expand_clusters(*rbd4.sc);
      if(reverse)
	rbd.sc->flip();
      if(if_celex.is_open()) {
	ec->set_standard(if_celex, rbd.get_vocabulary());
	rbd.write_stream(oss);
	write_params(argc, argv, of_dbg_dump);
	std::istringstream is(oss.str());
	ec->eval_all(is);
	rbd.write_debug_stream(of_dbg_dump);
      } else {
	rbd.write_stream(of_eval);
	write_params(argc, argv, of_dbg_dump);
	rbd.write_debug_stream(of_dbg_dump);
      }
    }
    break;
  case infl::EXP_DS_GC:
    {
      infl::RBD_DS_GC rbd(sf, input_dir.c_str());
      rbd.ep.confidence = confidence_threshold;
      rbd.ep.min_cell_size = min_cell_size;
      rbd.process_rule_by_doc();
      if(reverse)
	rbd.sc->flip();
      if(if_celex.is_open()) {
	ec->set_standard(if_celex, rbd.get_vocabulary());
	rbd.write_stream(oss);
	write_params(argc, argv, of_dbg_dump);
	std::istringstream is(oss.str());
	ec->eval_all(is);
	rbd.write_debug_stream(of_dbg_dump);
      } else {
	rbd.write_stream(of_eval);
	write_params(argc, argv, of_dbg_dump);
	rbd.write_debug_stream(of_dbg_dump);
      }
    }
    break;
  case infl::BASELINE_ALL_SINGLETON: 
    {
      infl::Baseline<infl::BASELINE_ALL_SINGLETON>
	bs(sf, input_dir.c_str());
      bs.process();
      if(if_celex.is_open()) {
	ec->set_standard(if_celex, bs.wb);
	bs.write_stream(oss);
	write_params(argc, argv, of_dbg_dump);
	std::istringstream is(oss.str());
	ec->eval_all(is);
      }
    }
    break;
  case infl::BASELINE_ALL_IN_ONE:
    {
      infl::Baseline<infl::BASELINE_ALL_IN_ONE>
	bs(sf, input_dir.c_str());
      bs.process();
      if(if_celex.is_open()) {
	ec->set_standard(if_celex, bs.wb);
	bs.write_stream(oss);
	write_params(argc, argv, of_dbg_dump);
	std::istringstream is(oss.str());
	ec->eval_all(is);
      }
    }
    break;
  case infl::BASELINE_FIRST_K:
    {
      infl::Baseline<infl::BASELINE_FIRST_K>
	bs(sf, input_dir.c_str());
      bs.min_len = k_thresh;
      bs.process();
      if(reverse)
	bs.flip();
      if(if_celex.is_open()) {
	ec->set_standard(if_celex, bs.wb);
	bs.write_stream(oss);
	write_params(argc, argv, of_dbg_dump);
	std::istringstream is(oss.str());
	ec->eval_all(is);
      }
    }
    break;
  case infl::EXP_USP_DS_DC:
    {
      infl::RBD_USP_DS_DC rbd(input_dir.c_str());
      rbd.ep.confidence = confidence_threshold;
      rbd.ep.min_cell_size = min_cell_size;
      rbd.process_rule_by_doc();
      if(if_celex.is_open()) {
	ec->set_standard(if_celex,*rbd.vocabulary);
	rbd.write_stream(oss);
	write_params(argc, argv, of_dbg_dump);
	std::istringstream is(oss.str());
	ec->eval_all(is);
	rbd.write_debug_stream(of_dbg_dump);
      } else {
	rbd.write_stream(of_eval);
	write_params(argc, argv, of_dbg_dump);
	rbd.write_debug_stream(of_dbg_dump);
      }
    }
    break;
  case infl::EXP_USP_GS_DC:
    {
      infl::RBD_USP_GS_DC rbd(input_dir.c_str(), big_file.c_str());
      rbd.ep.confidence = confidence_threshold;
      rbd.ep.min_cell_size = min_cell_size;
      rbd.process_rule_by_doc();
//       infl::RBD_USP_GS_DC rbd4(input_dir.c_str());
//       rbd4.process_rule_by_doc();
//       rbd.sc->expand_clusters(*rbd4.sc);
      if(if_celex.is_open()) {
	rbd.write_stream(oss);
	write_params(argc, argv, of_dbg_dump);
	std::istringstream is(oss.str());
	ec->eval_all(is);
	rbd.write_debug_stream(of_dbg_dump);
      } else {
	rbd.write_stream(of_eval);
	write_params(argc, argv, of_dbg_dump);
	rbd.write_debug_stream(of_dbg_dump);
      }
    }
    break;
  case infl::EXP_USP_DS_GC:
    {
      infl::RBD_USP_DS_GC rbd(input_dir.c_str());
      rbd.ep.confidence = confidence_threshold;
      rbd.ep.min_cell_size = min_cell_size;
      rbd.process_rule_by_doc();
      if(if_celex.is_open()) {
	rbd.write_stream(oss);
	write_params(argc, argv, of_dbg_dump);
	std::istringstream is(oss.str());
	ec->eval_all(is);
	rbd.write_debug_stream(of_dbg_dump);
      } else {
	rbd.write_stream(of_eval);
	write_params(argc, argv, of_dbg_dump);
	rbd.write_debug_stream(of_dbg_dump);
      }
    }
    break;
  default:
    break;
  }
  delete ec;
  delete sf;
}
