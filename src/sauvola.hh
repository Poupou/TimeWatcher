#ifndef SAUVOLA_HH_
# define SAUVOLA_HH_

# include <opencv2/opencv.hpp>

int mean(const cv::Mat& integral, int x, int y, int w);
int deviation(const cv::Mat& src, int x, int y);
int threshold(const cv::Mat& src, const cv::Mat& integral, int x, int y,
              double k, int w);
void otsu(const cv::Mat& src, cv::Mat& dst, int threshold);
void sauvola(const cv::Mat& src, cv::Mat& dst, int window, double k);

# endif // !SAUVOLA_HH_
