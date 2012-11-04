#include <iostream>
#include <vector>
#include <math.h>

#include <opencv2/opencv.hpp>

int main(int argc, char** argv)
{
	std::cout << "Hello World!" << std::endl;

#ifdef DEBUG
	std::cout << "Hello World! (with debug)" << std::endl;
#endif

  std::string filename (argv[1]);
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

  cv::imwrite("out.png", img);

  return 0;
}
