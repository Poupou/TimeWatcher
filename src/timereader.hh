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


std::ostream& operator<< (std::ostream& ostr, std::pair<int, int>& hour);
hourvector readclock(std::vector<Mask> parMasks, std::string& parFilePath);



#endif // !TIME_READER_HH_
