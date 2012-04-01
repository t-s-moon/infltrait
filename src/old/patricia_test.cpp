#include <sstream>
#include <ctime>

#include "patricia.hpp"

int main(int argc, char* argv[])
{
//   list<UINT> a1, a2;
//   for(UINT i = 0; i < 10; i++)
//     {
//       a1.insert(a1.end(), i);
//     }

//   for(list<UINT>::iterator it = a1.begin(); it != a1.end(); it++)
//     {
//       cout << *it << endl;
//       a1.erase(it);
//     }

//   time_t* timer = new time_t;
//   tm* st_tm = new tm;

//   for(UINT i = 0; i < 10; i++)
//     {
//       time(timer);
//       localtime_r(timer, st_tm);
//       cout << st_tm->tm_sec << endl;
//       sleep(1);
//     }
  
//   delete timer;
//   delete st_tm;

//   init_file_name();
//   Patricia p;
//   p.close();

  //use example ein einem eieieeidieneineeinemeieieeieneinemeieieeieineieieieneinemdidkja
  string line;

  ifstream ifs(argv[1]);
  ofstream of("graph.dot");
  while(getline(ifs, line))
    {

      Patricia p;
      vector<string> vs;
      if(line != "") // remember to handle this!!
	{
	  istringstream iss(line);
	  string word;
	  while(iss >> word)
	    {
	      vs.push_back(word);
	    }
	  try
	    {
	      for(UINT i = 0; i < vs.size(); i++)
		{
		  p.insert(vs[i], 1);
		}
	      p.graphVizPrint(of);
	    }
	  catch(Error& e)
	    {
	      cerr << e.m() << endl;
	    }
	}
    }
  of.close();
  ifs.close();

  Patricia p;
  p.close();
  
  return 0;
}
