#include <cmath>
#include "timereader.hh"
#include "clockreader.hh"

TTimeReader::TTimeReader(cv::Mat& parImg)
: FImage(parImg)
{
}

TTimeReader::TTimeReader()
{
}

cv::Mat
TTimeReader::GetSubImg(int parOriginX, int parOriginY,
		       int parWidth, int parHeight);
{
  cv::Mat res = cv::Mat(FImage, CvRect(parOriginX, parOriginY,
					  parWidth, parHeight));

  return res;
}

std::pair<int, int>
TTimeReader::GetHoursFromMask(Mask& parMask)
{
  cv::Mat clock = GetSubImg(parMask.x_ - parMask.major_rad_,
			    parMask.y_ - parMask.major_rad_,
			    parMask.major_rad_, parMask.major_rad_);

  cv::namedWindow("Clock", CV_WINDOW_AUTOSIZE);
  cv::imshow("Display clock", clock);
}
