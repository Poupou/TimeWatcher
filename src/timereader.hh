#ifndef TIME_READER_HH_
# define TIME_READER_HH_

# include <vector>
# include <cv.h>
# include "mask.hh"

typedef std::vector<std::pair<int, int> > hourvector;


class TTimeReader
{
  public:
    TTimeReader(cv::Mat& parImg);
    ~TTimeReader();

  public:
    cv::Mat GetSubImg(int parOriginX, int parOriginY,
		       int parWidth, int parHeight);
    std::pair<int, int> GetHoursFromMask(Mask& parMask);

  private:
    cv::Mat& FImage;
};


std::ostream& operator<< (std::ostream& ostr, std::pair<int, int>& hour);
hourvector readclock(std::vector<Mask> parMasks, std::string& parFilePath);



#endif // !TIME_READER_HH_
