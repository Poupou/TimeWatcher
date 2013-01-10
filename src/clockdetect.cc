#include "clockreader.hh"

std::vector<Mask> clock_detect(std::string& filename)
{
  cv::Mat img = cv::imread(filename, CV_LOAD_IMAGE_COLOR);
  cv::Mat gray;

  cv::cvtColor(img, gray, CV_BGR2GRAY);
//  cv::GaussianBlur(img, img, cv::Size(9, 9), 2, 2);

  cv::Mat img2;
  cv::Mat img3;

  //cv::Sobel(img, img2, -1, 1, 1);
  //cv::threshold(img2, img3, 1, 255, CV_THRESH_OTSU);
  cv::Canny(gray, gray, 150, 200, 3);

  std::vector<cv::Vec3f> circles;
  cv::HoughCircles(gray, circles, CV_HOUGH_GRADIENT, 1, img.rows / 10, 100, 100, 0, 0);

  std::cerr << "circles detected: " << circles.size() << std::endl;

  std::vector<Mask> masks;

  for (size_t i = 0; i < circles.size(); i++ )
  {
    cv::Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
    int radius = cvRound(circles[i][2]);

    cv::circle(img, center, 3, cv::Scalar(0,255,0), -1, 8, 0);
    cv::circle(img, center, radius, cv::Scalar(0,0,255), 3, 8, 0);

    Mask m(center.x, center.y, radius, radius, 0);
    masks.push_back(m); // TODO: vect of non-scalar objects :x
  }

  //cv::Sobel(img, img3, -1, 1, 2);
  //cv::add(img3, img2, img);
  //cv::Canny(img, img, 10, 100);

  //cv::imwrite("out1.png", img);
  //cv::imwrite("out2.png", img2);
  //cv::imwrite("out3.png", img3);
  cv::namedWindow("canny", CV_WINDOW_AUTOSIZE);
  cv::imshow("canny", gray);
  cv::waitKey();

  cv::namedWindow("circles", CV_WINDOW_AUTOSIZE);
  cv::imshow("circles", img);
  cv::waitKey();

  return masks;
}

