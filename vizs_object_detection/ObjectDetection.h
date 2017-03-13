#pragma once
#ifndef OBJECT_DETECTION
#define OBJECT_DETECTION
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/opencv.hpp"


using namespace cv;

class ObjectDetection
{
public:
	ObjectDetection();
	ObjectDetection(std::vector<std::vector<Point> > contours, Mat src);
	~ObjectDetection();
private:
	void setLabel(cv::Mat& im, const std::string label, std::vector<cv::Point>& contour, std::string objectShape);
	std::string getHSV(Mat hsvImg, std::vector<Point> contour);
	static double angle(cv::Point pt1, cv::Point pt2, cv::Point pt0);


};

#endif	