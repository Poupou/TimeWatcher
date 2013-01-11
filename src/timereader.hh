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
    bool is_clock();
    void evalutate_length();
    void isolate_clockwise(cv::Point point);

  private:
    unsigned int FHourAngle;
    unsigned int FMinuteAngle;
    std::vector<std::pair<unsigned int, unsigned int> > FAngles;
    cv::Mat& FImage;
    cv::Mat FClockImage;
    cv::Mat FDeletedClockwise;
    cv::Mat FIsolatedClockwise;
};


// std::ostream& operator<< (std::ostream& ostr, std::pair<int, int>& hour);
// hourvector readclock(std::vector<Mask> parMasks, std::string& parFilePath);
//
// <<<<<<< HEAD
//   return ostr <<
// 	      ((hour.first < 10) ? "0" : "") << hour.first
// 	      << ":" <<
// 	      ((hour.second < 10) ? "0" : "") << hour.second;
// }
//
// hourvector readclock(std::vector<Mask> parMasks,
// 		     std::string& parFilePath)
// {
//
//   hourvector hv;
//   std::pair<int, int> tst(12, 5);
//   hv.push_back(tst);
//
//   return hv;
// }
// =======
// >>>>>>> ed456d753eb8085a908f5d38c460e2d310f1ca1f

#endif // !TIME_READER_HH_
