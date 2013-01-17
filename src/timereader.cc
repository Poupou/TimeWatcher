#include <libgen.h>
#include <cmath>
#include "mask.hh"
#include "clockreader.hh"
#include "sauvola.hh"

#define PI 3.14159265359


TTimeReader::TTimeReader(cv::Mat& parImg, const std::string& path)
: FImage(parImg)
, path_(path)
{
}

TTimeReader::~TTimeReader()
{
}

/*
void
TTimeReader::sauvola(const cv::Mat& src, cv::Mat& dest, int mode)
{
  dest = src.clone();
  int border_size = floor(KERNEL_SIZE / 2.0);

  cv::Mat temp(src.size().width + border_size, src.size().height + border_size, CV_8UC1);
  temp.setTo(cv::Scalar(0, 0, 0,0));

  cv::copyMakeBorder(src, temp, border_size, border_size, border_size, border_size,
		     cv::BORDER_REPLICATE);

  for (int i = border_size; i < src.size().height + border_size; i++)
    for (int j = border_size; j < src.size().width + border_size; j++)
    {
      double m, s;
      uchar t, g, b;

      m = mean(temp, i, j, border_size);
      s = standard_deviation(temp, i, j, border_size, m);
      t = (uchar) (m * (1 + SAUVOLA_K_PARAM * (s / SAUVOLA_K_PARAM - 1)));

      g = temp.at<uchar>(cv::Point(i, j));
      if (g <= t)
      {
	if (mode == THRESH_BINARY)
	  b = 0;
	else
	  b = g;
      }
      else
	b = 255;

      uchar* pixel = dest.ptr(i - border_size, j - border_size);
      *pixel = b;
    }
}

double TTimeReader::mean(cv::Mat& img, int row, int col, int border_size)
{
  double total = 0.0;

  for (int i = row - border_size; i <= row + border_size; ++i)
    for (int j = col - border_size; j <= col + border_size; ++j)
    {
      uchar g = img.at<uchar>(cv::Point(i, j));
      total += g;
    }

  return total / (KERNEL_SIZE * KERNEL_SIZE);
}

double TTimeReader::standard_deviation(cv::Mat& img, int row, int col, int border_size, double m)
{
  double total = 0.0;

  for (int i = row - border_size; i <= row + border_size; ++i)
    for (int j = col - border_size; j <= col + border_size; ++j)
    {
      uchar g = img.at<uchar>(cv::Point(i, j));
      double deviation = 0.0;
      deviation = (double) g - m;
      total += deviation * deviation;
    }

  return sqrt(total / (KERNEL_SIZE * KERNEL_SIZE));
} */

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
TTimeReader::fill_clockwise(cv::Mat& filled_clockwise, cv::Point point)
{
  if (point.x < 0 || point.x >= filled_clockwise.size().width)
    return;
  if (point.y < 0 || point.y >= filled_clockwise.size().height)
    return;

  uchar* scale = filled_clockwise.ptr (point.x, point.y);

  if (*scale)
    return;

  *scale = 255;

  point.x++;
  fill_clockwise(filled_clockwise, point);
  point.x -= 2;
  fill_clockwise(filled_clockwise, point);
  point.x++;
  point.y++;
  fill_clockwise(filled_clockwise, point);
  point.y -= 2;
  fill_clockwise(filled_clockwise, point);
}

void
TTimeReader::isolate_clockwise()
{
  cv::Mat deletedClockwise = FClockImage.clone();

  for (int i = 0; i < deletedClockwise.size().width; ++i)
    for (int j = 0; j < deletedClockwise.size().height; j++)
    {
      int deltai = center_.x - i;
      int deltaj = center_.x - j;
      if (sqrt(deltai*deltai + deltaj*deltaj) < deletedClockwise.size().height / 8)
	*deletedClockwise.ptr(i, j) = 0;
    }

  cv::floodFill(deletedClockwise, center_, cv::Scalar(255, 255, 255));
//  fill_clockwise(deletedClockwise, center_);
  FIsolatedClockwise = deletedClockwise - FClockImage;

  for (int i = 0; i < FClockImage.size().width; i++)
    for (int j = 0; j < FClockImage.size().height; j++)
    {
      uchar* pixel = FIsolatedClockwise.ptr(i, j);
      if (*pixel == 255)
	*pixel = 0;
      else
	*pixel = 255;
    }



  cv::namedWindow("ClockWises", CV_WINDOW_AUTOSIZE);
  cv::imshow("ClockWises", FIsolatedClockwise);
  cv::waitKey(0);

  std::vector<int> compresion_param;
  compresion_param.push_back(CV_IMWRITE_PNG_COMPRESSION);
  compresion_param.push_back(9);
  std::string p = path_;
  p.append("isolated.jpg");
  cv::imwrite(p.c_str(), FIsolatedClockwise, compresion_param);
}

void
TTimeReader::show_work_circle(int dist)
{
  cv::Mat debug = FIsolatedClockwise.clone();
  cv::circle(debug, center_, r_ / dist, cv::Scalar(255, 255, 255), 1, 8, 0);

  cv::namedWindow("Canny img", CV_WINDOW_AUTOSIZE);
  cv::imshow("Canny img", debug);
  cv::waitKey(0);
}

void
TTimeReader::find_clock_wise(int dist)
{
  std::cerr << "NEW FIND CLOCK" << std::endl;
  float r = r_ / dist;

  FMinuteAngle = -1;
  FHourAngle = -1;
  unsigned int j = 0;
  unsigned int height = FIsolatedClockwise.size().height;
  unsigned int clockwisefound = 0;
  unsigned int enterAngle;
  int max = 0;
  int min = 10000;

  // Init : we find a position for the clock where we can
  // start rotation
  {
    unsigned int x = center_.x + r;
    unsigned int y = height - center_.y;
    while (j < 360 && !FIsolatedClockwise.at<char>(cv::Point(x, y)))
    {
      x = center_.x + cos(j * PI / 180.f) * r;
      y = height - (center_.y + sin(j * PI / 180.f) * r);
      j++;
    }
  }
  cv::imwrite("out1.png", FIsolatedClockwise);

  uchar previous = 255;
  j %= 360;
  cv::namedWindow("Canny img", CV_WINDOW_AUTOSIZE);

  for (unsigned int i = j + 1; i % 360 != j; ++i)
  {
    unsigned int x = center_.x + cos(double(i) * PI / 180.f) * r;
    unsigned int y = height - (center_.y + sin(double(i) * PI / 180.f) * r);
    uchar current = FIsolatedClockwise.at<uchar>(cv::Point(x, y));

    if ( current == 0 && previous == 255)
    {
      enterAngle = i;
      clockwisefound++;
      if (clockwisefound > 4)
      {
	FHourAngle = -1;
	FMinuteAngle = -1;
	return;
      }
    }

    if (current == 255 && previous == 0)
    {
      if (i - enterAngle < 30)
      {
	if ((i - enterAngle) > max)
	{
	  std::cerr << "hour angle =" << (i + enterAngle) / 2  << std::endl;
	  FMinuteAngle = FHourAngle;
	  max = i - enterAngle;
	  FHourAngle = (i + enterAngle) / 2;
	}
	else if (min == 10000 || i - enterAngle > min)
	{
	  std::cerr << "minute angle =" << (i + enterAngle) / 2  << std::endl;
	  FMinuteAngle = (i + enterAngle) / 2;
	  min = (i - enterAngle);
	}
      }
    }
    previous = current;
  }
}

bool
TTimeReader::close_angle(int angle1, int angle2, float pourcent) const
{
  if (angle1 == -1 || angle2 == -1)
    return false;

  if (angle1 == angle2)
    return true;

  if (angle1 < angle2 && angle1 + 360 * pourcent > angle2)
    return true;
  else if (angle1 > angle2 && angle1 - 360 * pourcent < angle2)
    return true;

  return false;
}

bool
TTimeReader::are_angle_closes(unsigned int i, unsigned int j)
{
  int delta = std::max(i, j) - std::min(i, j);

  return (delta < 10);
}


void
TTimeReader::show_clock_wise(int dist) const
{
  unsigned int height = FColoredClock.size().height;
  cv::Mat coloredclock = FColoredClock.clone();

  std::cerr << "FHourAngle="  << FHourAngle << std::endl;
  std::cerr << "FMinuteAngle="  << FMinuteAngle << std::endl;
  cv::Point hourpoint(center_.x + cos(double(FHourAngle) * PI / 180.f) * r_ / (dist + 1),
		      height - (center_.y + sin(double(FHourAngle) * PI / 180.f) * r_ / (dist + 1)));
  cv::Point minutepoint(center_.x + cos(double(FMinuteAngle) * PI / 180.f) * r_ / dist,
		      height - (center_.y + sin(double(FMinuteAngle) * PI / 180.f) * r_ / dist));
  line(coloredclock, center_, hourpoint, cv::Scalar(0, 255, 255), 5, CV_AA);
  line(coloredclock, center_, minutepoint, cv::Scalar(255, 0, 0), 3, CV_AA);

  cv::namedWindow("ClockWises", CV_WINDOW_AUTOSIZE);
  cv::imshow("ClockWises", coloredclock);

  std::string p = path_;
  p.append("_clockwises.jpg");
  std::vector<int> compresion_param;
  compresion_param.push_back(CV_IMWRITE_PNG_COMPRESSION);
  compresion_param.push_back(9);
  cv::imwrite(p.c_str(), coloredclock, compresion_param);
  cv::waitKey(0);
}

void
TTimeReader::is_clock(int hoursangles[3], int minuteangles[3])
{
}

void
TTimeReader::find_clockwise_from_angle()
{
  int hoursangles[3] = {-1, -1, -1};
  int minuteangles[3] = {-1, -1, -1};
  int nbourslessthan1 = 0;
  int nbminlessthan1= 0;

  int max = 5;
  for (int i = max; i >= max - 2; --i)
  {
    show_work_circle(i);
    find_clock_wise(i);
    hoursangles[max - i] = FHourAngle;
    minuteangles[max - i] = FMinuteAngle;
    if (FHourAngle == -1)
      nbourslessthan1++;
    if (FMinuteAngle == -1)
      nbminlessthan1++;

//    if (FMinuteAngle != -1 && FHourAngle != -1)
//      show_clock_wise(i);
  }

  FHourAngle = -1;
  FMinuteAngle = -1;

  float pourcent = 0.05f;

  is_clock_ = true;

  if (hoursangles[1] != -1 &&
      hoursangles[2] != -1 &&
      close_angle(hoursangles[1], hoursangles[2], pourcent))
    FHourAngle = (hoursangles[1] + hoursangles[2]) / 2;
  else if (hoursangles[0] != -1 &&
	   hoursangles[2] != -1 &&
	   close_angle(hoursangles[0], hoursangles[2], pourcent))
    FHourAngle = (hoursangles[0] + hoursangles[2]) / 2;
  else if (hoursangles[0] != -1 &&
	   hoursangles[1] != -1 &&
	   close_angle(hoursangles[0], hoursangles[1], pourcent))
    FHourAngle = (hoursangles[0] + hoursangles[1]) / 2;
  else if (nbourslessthan1 == 2)
  {
      for (int i = 0; i < 3; ++i)
      {
	if (hoursangles[i] != -1)
	  FHourAngle = hoursangles[i];
      }
  }
  else
    is_clock_ = false;

  if (minuteangles[1] != -1 &&
      minuteangles[2] != -1 &&
      close_angle(minuteangles[1], minuteangles[2], pourcent))
    FMinuteAngle = (minuteangles[1] + minuteangles[2]) / 2;
  else if (minuteangles[0] != -1 &&
	   minuteangles[2] != -1 &&
	   close_angle(minuteangles[0], minuteangles[2], pourcent))
    FMinuteAngle = (minuteangles[0] + minuteangles[2]) / 2;
  else if (minuteangles[0] != -1 &&
	   minuteangles[1] != -1 &&
	   close_angle(minuteangles[0], minuteangles[1], pourcent))
    FMinuteAngle = (minuteangles[0] + minuteangles[1]) / 2;

  else if (nbminlessthan1 == 2)
  {
      for (int i = 0; i < 3; ++i)
	if (minuteangles[i] != -1)
	  FMinuteAngle = minuteangles[i];
  }
  else
    is_clock_ = false;
}


std::pair<int, int>
TTimeReader::GetHoursFromMask(Mask& parMask)
{
  init_work_image(parMask);
  isolate_clockwise();
  find_clockwise_from_angle();

  cv::Mat out = FColoredClock.clone();
  for (int i = 5; i >= 3; --i)
    cv::circle(out, center_, r_ / i, cv::Scalar(255, 0, 0), 1, 8, 0);

  std::string p = path_;
  p.append("_circles.jpg");
  std::vector<int> compresion_param;
  compresion_param.push_back(CV_IMWRITE_PNG_COMPRESSION);
  compresion_param.push_back(9);
  cv::imwrite(p.c_str(), out, compresion_param);

  if (is_clock_)
    show_clock_wise(2);

  // Compute time
  int h = ((FHourAngle - 90) % 360) / 12;
  int m = ((FMinuteAngle - 90) % 360) / 60;

  return std::pair<int, int>(h, m);
}

void
TTimeReader::init_work_image(Mask& parMask)
{
  center_ = cv::Point(parMask.major_rad_, parMask.major_rad_);
  r_ = parMask.major_rad_;

  cv::Mat clock = GetSubImg(parMask.x_ - parMask.major_rad_,
			    parMask.y_ - parMask.major_rad_,
			    2 * parMask.major_rad_, 2 * parMask.major_rad_);

  std::vector<int> compresion_param;
  compresion_param.push_back(CV_IMWRITE_PNG_COMPRESSION);
  compresion_param.push_back(9);
  std::string p = path_;
  p.append("orig.jpg");
  cv::imwrite(p.c_str(), clock, compresion_param);

  FColoredClock = clock.clone();

  cv::cvtColor(clock, clock, CV_BGR2GRAY);

  // on supprime ce qui est en dehor du cercle
  for (int i = 0; i < clock.size().width; ++i)
    for (int j = 0; j < clock.size().height; j++)
    {
      int deltai = center_.x - i;
      int deltaj = center_.x - j;
      if (sqrt(deltai*deltai + deltaj*deltaj) > parMask.major_rad_)
	*clock.ptr(i, j) = 255;
//      if (sqrt(deltai*deltai + deltaj*deltaj) < parMask.major_rad_ / 8)
//	*clock.ptr(i, j) = 0;
    }

//  cv::equalizeHist(clock, clock);
//  cv::threshold(clock, clock, 70, 255, cv::THRESH_BINARY);
  cv::Mat canny;
  cv::Canny(clock, canny, 100, 200, 3);

  cv::namedWindow("Canny img", CV_WINDOW_AUTOSIZE);
  cv::imshow("Canny img", canny);
  cv::waitKey(0);
  sauvola(clock, clock, clock.size().height / 8, 0.01);
  p = path_;
  p.append("sauvola.jpg");
  cv::imwrite(p.c_str(), clock, compresion_param);

//  sauvola(clock, clock, THRESH_GRAYSCALE);

  cv::Mat cannyimg, cdst;
//
  unsigned int erodesize = 3;
  unsigned int dilatesize = 3;
  cv::Mat erodeElement = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(erodesize * 2 + 1, erodesize * 2 + 1), cv::Point(erodesize, erodesize));
  cv::Mat dilateElement = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(dilatesize * 2 + 1, dilatesize * 2 + 1), cv::Point(dilatesize, dilatesize));
//
  cv::erode(clock, clock, erodeElement);
  cv::dilate(clock, clock, dilateElement);
//  //cv::imshow("Canny img", clock);
//  //cv::waitKey(0);
//  //cv::imshow("Canny img", clock);
//  //cv::waitKey(0);

  cv::namedWindow("Canny img", CV_WINDOW_AUTOSIZE);
  cv::imshow("Canny img", clock);
  cv::waitKey(0);

  FClockImage = clock;
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

hourvector readclock(std::vector<Mask> masks,
		     std::string& parFilePath)
{
  cv::Mat origImage = cv::imread(parFilePath, CV_LOAD_IMAGE_COLOR);
  hourvector r;
  auto it = masks.begin();
  auto end = masks.end();

  while (it != end)
  {
    std::cerr << *it << std::endl;
    if (it->major_rad_ > 40)
    {
      TTimeReader tr = TTimeReader(origImage, parFilePath);
      it->major_rad_ = reduct_x_pourcent(it->major_rad_, 20);
      r.push_back(tr.GetHoursFromMask(*it));
    }
    ++it;
  }

//  Mask m(1420, 1408, 480, 484, 0); // bigben-1 NOK
// Mask m(198, 132, 96, 96, 0); // basic-1 OK

 //Mask m(474, 484, 400, 400, 0); // basic-2 OK
 //Mask m(1674, 1150, 326, 326, 0); // TIRF/5.jpg OK
 // Mask m(1216, 978, 231, 231, 0); // TIRF/6.jpg processed OK
//  Mask m(1209, 960, 210, 210, 0); // TIRF/6.jpg OK

 // Mask m(1035, 993, 218, 218, 0); // TIRF/13.jpg OK
 // Mask m(1125, 696, 132, 132, 0); // TIRF/15.jpg OK
 // Mask m(1696, 394, 178, 178, 0); // TIRF/17.jpg // NOK
 //
 //     TTimeReader tr = TTimeReader(origImage, parFilePath);
 //     m.major_rad_ = reduct_x_pourcent(m.major_rad_, 20);
 //     tr.GetHoursFromMask(m);

  return r;
}
