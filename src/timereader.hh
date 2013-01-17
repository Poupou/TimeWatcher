#ifndef TIME_READER_HH_
# define TIME_READER_HH_

# include <vector>
//# include <cv.h>
# include <opencv2/opencv.hpp>
# include "mask.hh"

typedef std::vector<std::pair<int, int> > hourvector;

std::ostream& operator<< (std::ostream& ostr, std::pair<int, int>& hour);
hourvector readclock(std::vector<Mask> parMasks,
                     std::string& parFilePath,
                     hourvector& result);

class TTimeReader
{
  public:
    TTimeReader(cv::Mat& parImg, const std::string& path);
    ~TTimeReader();

  public:
    cv::Mat GetSubImg(int parOriginX, int parOriginY,
		       int parWidth, int parHeight);
    std::pair<int, int> GetHoursFromMask(Mask& parMask, hourvector& hours);

  private:
    //double mean(cv::Mat& img, int row, int col, int border_size);
    //double standard_deviation(cv::Mat& img, int row, int col, int border_size, double m);
    //void sauvola(const cv::Mat& src, cv::Mat& dest, int mode);
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
    bool are_angle_closes(unsigned int i, unsigned int j);
    void is_clock(int hoursangles[3], int minuteangles[3]);

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
    std::string path_;
};


#endif // !TIME_READER_HH_
