#ifndef TIME_READER_HH_
# define TIME_READER_HH_

# include <vector>
//# include <cv.h>
# include <opencv2/opencv.hpp>
# include "mask.hh"

typedef std::vector<std::pair<int, int> > hourvector;

std::ostream& operator<< (std::ostream& ostr, std::pair<int, int>& hour);
hourvector readclock(std::vector<Mask> parMasks, std::string& parFilePath);

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
    void init_work_image(Mask& parMask);
    bool close_angle(int angle1, int angle2, float pourcent) const;
    void find_clock_wise(int dist);
    void show_work_circle(int dist);
    void show_clock_wise(int dist) const;
    void fill_clockwise(cv::Mat& filled_clokwise, cv::Point point);
    bool is_clock();
    void evalutate_length();
    void isolate_clockwise();
    void find_clockwise_from_angle();

  private:
    bool is_clock_;
    int FHourAngle;
    int FMinuteAngle;
    float r_;
    cv::Point center_;
    std::vector<std::pair<unsigned int, unsigned int> > FAngles;
    cv::Mat& FImage;
    cv::Mat FClockImage;
    cv::Mat FIsolatedClockwise;
    cv::Mat FColoredClock;
};


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
