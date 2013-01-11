#include <cmath>
#include "mask.hh"
#include "clockreader.hh"

#define PI 3.14159265359

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

void
TTimeReader::isolate_clockwise(cv::Point point)
{
  uchar* scale = FDeletedClockwise.ptr (point.x, point.y);

  if (*scale)
    return;

  *scale = 255;

  point.x++;
  isolate_clockwise(point);
  point.x -= 2;
  isolate_clockwise(point);
  point.x++;
  point.y++;
  isolate_clockwise(point);
  point.y -= 2;
  isolate_clockwise(point);

}


std::pair<int, int>
TTimeReader::GetHoursFromMask(Mask& parMask)
{
  cv::Point center(parMask.major_rad_, parMask.major_rad_);

  cv::Mat clock = GetSubImg(parMask.x_ - parMask.major_rad_,
			    parMask.y_ - parMask.major_rad_,
			    2 * parMask.major_rad_, 2 * parMask.major_rad_);
  cv::Mat coloredclock = clock.clone();

  cv::cvtColor(clock, clock, CV_BGR2GRAY);
//  cv::Canny(clock, clock, 100, 200, 5);
  cv::threshold(clock, clock, 127, 255, cv::THRESH_BINARY);
  cv::Mat cannyimg, cdst;

  unsigned int erodesize = 3;
  unsigned int dilatesize = 1;
  cv::Mat erodeElement = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(erodesize * 2 + 1, erodesize * 2 + 1), cv::Point(erodesize, erodesize));
  cv::Mat dilateElement = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(dilatesize * 2 + 1, dilatesize * 2 + 1), cv::Point(dilatesize, dilatesize));

  cv::dilate(clock, clock, dilateElement);
  cv::imshow("Canny img", clock);
  cv::waitKey(0);
  cv::erode(clock, clock, erodeElement);
  cv::imshow("Canny img", clock);
  cv::waitKey(0);

  cv::namedWindow("Canny img", CV_WINDOW_AUTOSIZE);
  cv::imshow("Canny img", clock);
  cv::waitKey(0);

  cv::Mat debug = clock.clone();
  cv::circle(debug, center, parMask.major_rad_ / 3, cv::Scalar(255, 255, 255), 1, 8, 0);

  cv::namedWindow("Canny img", CV_WINDOW_AUTOSIZE);
  cv::imshow("Canny img", debug);
  cv::waitKey(0);

  FClockImage = clock.clone();
  FIsolatedClockwise = FClockImage.clone();
  FDeletedClockwise = FClockImage.clone();

//  isolate_clockwise(center);

  FIsolatedClockwise = FDeletedClockwise - FClockImage;

  cv::namedWindow("Canny img", CV_WINDOW_AUTOSIZE);
  cv::imshow("Canny img", FIsolatedClockwise);
  cv::waitKey(0);

  for (int i = 0; i < FIsolatedClockwise.size().width; i++)
    for (int j = 0; j < FIsolatedClockwise.size().height; j++)
    {
      uchar* pixel = FIsolatedClockwise.ptr(i, j);
      if (*pixel == 255)
	*pixel = 0;
      else
	*pixel = 255;
    }

  cv::namedWindow("Canny img", CV_WINDOW_AUTOSIZE);
  cv::imshow("Canny img", FIsolatedClockwise);
  cv::waitKey(0);

  float r = parMask.major_rad_ / 4;
  int j = 0;
  unsigned int height = FClockImage.size().height;
  unsigned int x = center.x + r;
  unsigned int y = height - center.y;

  while (j < 360 && !FClockImage.at<char>(cv::Point(x, y)))
  {
    x = center.x + cos(j * PI / 180.f) * r;
    y = height - (center.y + sin(j * PI / 180.f) * r);
    j++;
  }
  cv::imwrite("out1.png", FClockImage);

  uchar previous = 255;
  unsigned int enterAngle;
  int clockwisefound = 0;
  cv::namedWindow("Canny img", CV_WINDOW_AUTOSIZE);
  unsigned int max = 0;
  unsigned int min = 10000;

  for (unsigned int i = j + 1; i % 360 != j; ++i)
  {
    unsigned int x = center.x + cos(double(i) * PI / 180.f) * r;
    unsigned int y = height - (center.y + sin(double(i) * PI / 180.f) * r);
    uchar current = FClockImage.at<uchar>(cv::Point(x, y));

    if ( current == 0 && previous == 255)
    {
      enterAngle = i;
      clockwisefound++;
    }

    if (current == 255 && previous == 0)
    {
      unsigned int x = center.x + cos(double(i + 1) * PI / 180.f) * r;
      unsigned int y = height - (center.y + sin(double(i + 1) * PI / 180.f) * r);

      if ((i - enterAngle) > max)
      {
	FMinuteAngle = FHourAngle;
	max = i - enterAngle;
	FHourAngle = (i + enterAngle) / 2;
      }
      else if (min == 10000 || i - enterAngle > min)
      {
	FMinuteAngle = (i + enterAngle) / 2;  
	min = (i - enterAngle);
      }
    }

    previous = current;
  }

  if (clockwisefound != 3)
    std::cerr << "clockwisefound=" << clockwisefound << std::endl;

  cv::Point hourpoint(center.x + cos(double(FHourAngle) * PI / 180.f) * r,
		      height - (center.y + sin(double(FHourAngle) * PI / 180.f) * r));
  cv::Point minutepoint(center.x + cos(double(FMinuteAngle) * PI / 180.f) * r,
		      height - (center.y + sin(double(FMinuteAngle) * PI / 180.f) * r));
  line(coloredclock, center, hourpoint, cv::Scalar(0, 255, 255), 5, CV_AA);
  line(coloredclock, center, minutepoint, cv::Scalar(255, 0, 0), 3, CV_AA);

  cv::namedWindow("Canny img", CV_WINDOW_AUTOSIZE);
  cv::imshow("Canny img", coloredclock);
  cv::waitKey(0);

  //for( size_t i = 0; i < lines.size(); i++)
  //{
  //  float rho = lines[i][0], theta = lines[i][1];
  //  cv::Point pt1, pt2;
  //  double a = cos(theta), b = sin(theta);
  //  double pente = a / b;
  //  double x0 = a*rho, y0 = b*rho;
  //  pt1.x = cvRound(x0 + 1000*(-b));
  //  pt1.y = cvRound(y0 + 1000*(a));
  //  pt2.x = cvRound(x0 - 1000*(-b));
  //  pt2.y = cvRound(y0 - 1000*(a));


  //  int diffx = pt1.x - center.x;


  //  if (areClose(pt1.y + pente * diffx, center.y, 1))
  //  {
  //    line(cdst, pt1, pt2, cv::Scalar(0,0,255), 1, CV_AA);
  //    cv::imshow("Lines", cdst);

  //    cv::waitKey(0);
  //  }

  //}
  return std::pair<int, int>(0,0);

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
  Mask m(1420, 1408, 480, 484, 0); // bigben-1 NOK
//  Mask m(198, 132, 96, 96, 0); // basic-1 OK

 // Mask m(474, 484, 400, 400, 0); // basic-2 OK
//  Mask m(1674, 1150, 326, 326, 0); // TIRF/5.jpg OK
  //Mask m(1209, 960, 210, 210, 0); // TIRF/6.jpg OK
  //Mask m(1035, 993, 218, 218, 0); // TIRF/13.jpg OK
  //Mask m(1125, 696, 132, 132, 0); // TIRF/15.jpg OK
  //Mask m(1696, 394, 178, 178, 0); // TIRF/17.jpg // NOK

  m.major_rad_ = reduct_x_pourcent(m.major_rad_, 0);
  m.minor_rad_ = reduct_x_pourcent(m.minor_rad_, 0);
  tr.GetHoursFromMask(m);

  return hourvector();
}
