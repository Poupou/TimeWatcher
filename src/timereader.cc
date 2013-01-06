#include <cmath>
#include "mask.hh"
#include "clockreader.hh"

TTimeReader::TTimeReader(cv::Mat& parImg)
: FImage(parImg)
{
}

TTimeReader::~TTimeReader()
{
}

cv::Mat
TTimeReader::GetSubImg(int parOriginX, int parOriginY,
		       int parWidth, int parHeight)
{
  cv::Mat res = cv::Mat(FImage, cvRect(parOriginX, parOriginY,
					  parWidth, parHeight));

  return res;
}

bool areClose(double a, double b, float pourcent)
{
  double delta = a - b;

  double absdelta = delta < 0 ? -delta : delta;

//  std::cerr << "areClose:" << (100 * absdelta / a) << std::endl;

  if (a < 0)
    return false;

  return ((100 * absdelta) / a < pourcent);
}


std::pair<int, int>
TTimeReader::GetHoursFromMask(Mask& parMask)
{
  cv::Point center(parMask.major_rad_, parMask.major_rad_);

  cv::Mat clock = GetSubImg(parMask.x_ - parMask.major_rad_,
			    parMask.y_ - parMask.major_rad_,
			    2 * parMask.major_rad_, 2 * parMask.major_rad_);

  cv::cvtColor(clock, clock, CV_BGR2GRAY);
  cv::Mat cannyimg, cdst;

  cv::Canny(clock, clock, 50, 200, 3);
  cv::cvtColor(clock, cdst, CV_GRAY2BGR);

  unsigned int erodesize = 1;
  unsigned int dilatesize = 1;
  cv::Mat erodeElement = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(erodesize * 2 + 1, erodesize * 2 + 1), cv::Point(erodesize, erodesize));
  cv::Mat dilateElement = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(dilatesize * 2 + 1, dilatesize * 2 + 1), cv::Point(dilatesize, dilatesize));

  cv::namedWindow("Canny img", CV_WINDOW_AUTOSIZE);
  cv::imshow("Canny img", clock);
  cv::waitKey(0);
  cv::dilate(clock, clock, dilateElement);
  cv::imshow("Canny img", clock);
  cv::waitKey(0);
  cv::erode(clock, clock, erodeElement);
  cv::imshow("Canny img", clock);
  cv::waitKey(0);

  std::vector<cv::Vec2f> lines;

  int i = 1;
  int count = 0;

  do
  {
    cv::HoughLines(clock, lines, 1, CV_PI / 180, (parMask.major_rad_) / i, 0, 0);
    count = 0;
    for( size_t i = 0; i < lines.size(); i++ )
    {
      float rho = lines[i][0], theta = lines[i][1];
      cv::Point pt1, pt2;
      double a = cos(theta), b = sin(theta);
      double pente = a / b;
      double x0 = a*rho, y0 = b*rho;
      pt1.x = cvRound(x0 + 1000*(-b));
      pt1.y = cvRound(y0 + 1000*(a));
      pt2.x = cvRound(x0 - 1000*(-b));
      pt2.y = cvRound(y0 - 1000*(a));


      int diffx = pt1.x - center.x;


      if (areClose(pt1.y + pente * diffx, center.y, 1))
	count++;
    }

    if (count < 3)
      i++;
    if (count > 6)
      i--;

    if (i == 0)
      break;

    std::cerr << "count =" << count << "\ti == " << i <<  std::endl;
  }
  while (count < 3 || count > 6);

  cv::namedWindow("Lines", CV_WINDOW_AUTOSIZE);

  for( size_t i = 0; i < lines.size(); i++ )
  {
    float rho = lines[i][0], theta = lines[i][1];
    cv::Point pt1, pt2;
    double a = cos(theta), b = sin(theta);
    double pente = a / b;
    double x0 = a*rho, y0 = b*rho;
    pt1.x = cvRound(x0 + 1000*(-b));
    pt1.y = cvRound(y0 + 1000*(a));
    pt2.x = cvRound(x0 - 1000*(-b));
    pt2.y = cvRound(y0 - 1000*(a));


    int diffx = pt1.x - center.x;


    if (areClose(pt1.y + pente * diffx, center.y, 1))
    {
      line(cdst, pt1, pt2, cv::Scalar(0,0,255), 1, CV_AA);
      cv::imshow("Lines", cdst);

      cv::waitKey(0);
    }

  }

}

std::ostream& operator<< (std::ostream& ostr, std::pair<int, int>& hour)
{
  return ostr <<
	      ((hour.first < 10) ? "0" : "") << hour.first
	      << "" <<
	      ((hour.second < 10) ? "0" : "") << std::endl;
}

unsigned int reduct_x_pourcent(int a, int pourcent)
{
  return (a - (a * pourcent / 100));
}

hourvector readclock(std::vector<Mask> parMasks,
		     std::string& parFilePath)
{
  cv::Mat origImage = cv::imread(parFilePath, CV_LOAD_IMAGE_COLOR);

  TTimeReader tr = TTimeReader(origImage);
  Mask m(1420, 1408, 480, 484, 0); // bigben-1
  //Mask m(198, 132, 96, 96, 0); // basic-1

  //Mask m(474, 484, 400, 400, 0); // basic-2

  m.major_rad_ = reduct_x_pourcent(m.major_rad_, 50);
  m.minor_rad_ = reduct_x_pourcent(m.minor_rad_, 50);
  tr.GetHoursFromMask(m);

  return hourvector();
}
