#include <iostream>

#include <opencv2/opencv.hpp>

int main(int argc, char** argv)
{
	std::cout << "Hello World!" << std::endl;

#ifdef DEBUG
	std::cout << "Hello World! (with debug)" << std::endl;
#endif

  std::string filename (argv[1]);
  cv::Mat img = cv::imread(filename, 0);

  cv::namedWindow("image", CV_WINDOW_AUTOSIZE);
  cv::imshow("image", img);
  cv::waitKey();
  return 0;
}
