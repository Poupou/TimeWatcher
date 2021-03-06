#include "clockreader.hh"

void display(std::string name, cv::Mat& img)
{
  cv::namedWindow(name, CV_WINDOW_AUTOSIZE);
  cv::imshow(name, img);
  cv::waitKey();
}

std::vector<Mask> clock_detect(std::string& filename)
{
  std::vector<Mask> masks;

  cv::Mat img = cv::imread(filename, CV_LOAD_IMAGE_COLOR);
  cv::Mat gray;
  cv::Mat binarized;
  cv::Mat blurred;

  cv::cvtColor(img, gray, CV_BGR2GRAY);
  cv::GaussianBlur(gray, blurred, cv::Size(9, 9), 2, 2);

  cv::threshold(gray, binarized, 1, 255, CV_THRESH_OTSU);

  cv::imwrite("gray.png", gray);
  cv::imwrite("binarized.png", binarized);

  cv::Mat img2;
  cv::Mat img3;

  //cv::Sobel(img, img2, -1, 1, 1);
  //cv::Canny(gray, gray, 150, 200, 3);

  std::vector<cv::Vec3f> circles;
  cv::HoughCircles(blurred, circles, CV_HOUGH_GRADIENT, 1, img.rows / 10, 100, 50, 0, 0);

  std::cerr << "circles detected: " << circles.size() << std::endl;

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

  cv::imwrite("circles.png", img);
  cv::namedWindow("Circles", CV_WINDOW_AUTOSIZE);
  cv::imshow("Circles", img);
  cv::waitKey(0);

  //cv::imwrite("out2.png", img2);
  //cv::imwrite("out3.png", img3);

  return masks;
}

