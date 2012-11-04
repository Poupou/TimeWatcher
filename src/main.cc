#include "clockreader.hh"

int main(int argc, char** argv)
{
/*  std::string filename (argv[1]);
  cv::Mat img = cv::imread(filename, 0);
  cv::Mat gray;

  //cv::cvtColor(img, gray, CV_BGR2GRAY);
  gray = img;
  cv::GaussianBlur( gray, gray, cv::Size(9, 9), 2, 2 );
  std::vector<cv::Vec3f> circles;
  cv::HoughCircles(gray, circles, CV_HOUGH_GRADIENT, 2, gray.rows/4, 200, 100);

  // Render centers & outline
  for( size_t i = 0; i < circles.size(); i++ )
  {
       cv::Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
       int radius = cvRound(circles[i][2]);

       circle(img, center, 3, cv::Scalar(0,255,0), -1, 8, 0);
       circle(img, center, radius, cv::Scalar(0,0,255), 3, 8, 0);
  }

  cv::namedWindow("circles", CV_WINDOW_AUTOSIZE);
  cv::imshow("circles", img);
  cv::waitKey();

  cv::imwrite("out.png", img); */

  if (argc != 3)
  {
    std::cerr << "Wrong number of argument:" << std::endl
	      << "expected: ./clockreader [mode] path/to/img" << std::endl;

    return 1;
  }


  std::string filename(argv[2]);
  std::string mode(argv[1]);


  if (mode == "detect")
  {
    std::vector<Mask> masks = clock_detect(filename);

    std::vector<Mask>::iterator end = masks.end();
    std::vector<Mask>::iterator it = masks.begin();

    for (; it != end; ++it)
      std::cout << (*it) << std::endl;

    return 0;
  }
  else if (mode == "reader")
  {
    std::vector<Mask> masks = read_masks();

    hourvector hours = readclock(masks, filename);

    hourvector::iterator end = hours.end();
    hourvector::iterator it = hours.begin();

    for (; it != end; ++it)
      std::cout << *it << std::endl;
  }
  else if (mode == "full")
  {
    std::vector<Mask> masks = clock_detect(filename);

    hourvector hours = readclock(masks, filename);

    hourvector::iterator end = hours.end();
    hourvector::iterator it = hours.begin();

    for (; it != end; ++it)
      std::cout << *it << std::endl;
  }
  else
  {
    std::cerr << "Mode not found!" << std::endl;
    return 2;
  }

  return 0;
}
