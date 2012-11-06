#ifndef TIME_READER_HH_
# define TIME_READER_HH_

# include <vector>

typedef std::vector<std::pair<int, int> > hourvector;


std::ostream& operator<< (std::ostream& ostr, std::pair<int, int>& hour)
{
  return ostr <<
	      ((hour.first < 10) ? "0" : "") << hour.first
	      << ":" <<
	      ((hour.second < 10) ? "0" : "") << hour.second;
}

hourvector readclock(std::vector<Mask> parMasks,
		     std::string& parFilePath)
{

  hourvector hv;
  std::pair<int, int> tst(12, 5);
  hv.push_back(tst);

  return hv;
}



#endif // !TIME_READER_HH_
