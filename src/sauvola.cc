#include <iostream>
#include "sauvola.hh"
#include <opencv2/opencv.hpp>

using namespace cv;

int mean(const cv::Mat& integral, int x, int y, int w)
{
    int inte1 = integral.at<int>(std::min(integral.rows - 1, y + w / 2),
                                 std::min(integral.cols - 1, x + w / 2));
    int inte2 = integral.at<int>(std::max(0, y - w / 2),
                                 std::max(0, x - w / 2));
    int inte3 = integral.at<int>(std::max(0, y - w / 2),
                                 std::min(integral.cols - 1, x + w / 2));
    int inte4 = integral.at<int>(std::min(integral.rows - 1, y + w / 2),
                                 std::max(0, x - w / 2));

    return ((inte1 + inte2 - inte3 - inte4) / (w * w));
}

int deviation(const cv::Mat& src, int x, int y)
{
    int cur = 2 * src.at<uchar>(y, x);
    int d_x = -src.at<uchar>(y, std::max(0, x - 1));
    int d_y = -src.at<uchar>(std::max(0, y - 1), x);

    return (cur + d_x + d_y);
}

int threshold(const cv::Mat& src, const cv::Mat& integral, int x, int y,
              double k, int w)
{
    int res = static_cast<int>(mean(integral, x, y, w)
            * (1 + k * ((deviation(src, x, y) / 128) - 1)));

    return (res);
}

void otsu(const cv::Mat& src, cv::Mat& dst, int threshold)
{
    for (int y = 0; y < src.rows; ++y)
        for (int x = 0; x < src.cols; ++x)
            dst.at<uchar>(y, x) = src.at<uchar>(y, x) < threshold ? 0 : 255;
}

void sauvola(const cv::Mat& src, cv::Mat& dst, int window, double k)
{
    char val = 0;
    cv::Mat integral;

    cv::integral(src, integral);

    for (int y = 0; y < src.rows; ++y)
        for (int x = 0; x < src.cols; ++x)
        {
            val = src.at<uchar>(y, x)
                < threshold(src, integral, x, y, k, window) ? 0 : 255;
            dst.at<uchar>(y, x) = val;
        }
}

