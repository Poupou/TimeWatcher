#ifndef TIME_READER_HH_
# define TIME_READER_HH_

# include <vector>

typedef std::vector<std::pair<int, int> > hourvector;


class TTimeReader
{
  public:
    TTimeReader(cv::Mat& parImg);
    ~TTimeReader();

  public:
    std::pair<int, int> GetHoursFromMask(Mask& parMask);

  private:
    cv::Mat& FImage;
};


std::ostream& operator<< (std::ostream& ostr, std::pair<int, int>& hour)
{
  return ostr <<
	      ((hour.first < 10) ? "0" : "") << hour.first
	      << "" <<
	      ((hour.second < 10) ? "0" : "") << std::endl;
}

hourvector readclock(std::vector<Mask> parMasks,
		     std::string& parFilePath)
{
  return hourvector();
}



#endif // !TIME_READER_HH_
