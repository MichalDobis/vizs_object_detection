#include "ObjectDetection.h"



ObjectDetection::ObjectDetection()
{
}

ObjectDetection::ObjectDetection(std::vector<std::vector<Point> > contours, Mat src) {

	std::vector<cv::Point> approx;
	cv::Mat dst = src.clone();
	cv::Mat hsvImg;

	cvtColor(src, hsvImg, CV_BGR2HSV);

	std::vector<std::vector<Point> > detectedContours;

	

	for (int i = 0; i < contours.size(); i++)
	{
		// Approximate contour with accuracy proportional
		// to the contour perimeter
		cv::approxPolyDP(cv::Mat(contours[i]), approx, cv::arcLength(cv::Mat(contours[i]), true)*0.03, true);

		// Skip small or non-convex objects 
		if (std::fabs(cv::contourArea(contours[i])) < 100 || !cv::isContourConvex(approx))
			continue;
	
		if (approx.size() == 3)
		{

			//cv::Rect r = cv::boundingRect(contours[i]);

			//cv::Point pt(r.x + ((r.width - text.width) / 2), r.y + ((r.height + text.height) / 2));



			/*hsv.val[0] = 0;
			hsv.val[1] = 0;
			hsv.val[2] = 0;

			int cSize = contours[i].size();
			for (int j = 0; j < cSize; j++) {
				hsv.val[0] += hsvImg.at<Vec3b>(contours[i][j]).val[0];
				hsv.val[1] += hsvImg.at<Vec3b>(contours[i][j]).val[1];
				hsv.val[2] += hsvImg.at<Vec3b>(contours[i][j]).val[2];
			}
			hsv.val[0] /= cSize;
			hsv.val[1] /= cSize;
			hsv.val[2] /= cSize;*/

			setLabel(dst, "TRI", contours[i],  getHSV(hsvImg, contours[i]));    // Triangles
			detectedContours.push_back(contours[i]);

			//drawContours(dst, contours[i], -1, (255, 0, 0), 3);

		}
		else if (approx.size() >= 4 && approx.size() <= 6)
		{
			// Number of vertices of polygonal curve
			int vtc = approx.size();

			// Get the cosines of all corners
			std::vector<double> cos;
			for (int j = 2; j < vtc + 1; j++)
				cos.push_back(angle(approx[j%vtc], approx[j - 2], approx[j - 1]));

			// Sort ascending the cosine values
			std::sort(cos.begin(), cos.end());

			// Get the lowest and the highest cosine
			double mincos = cos.front();
			double maxcos = cos.back();

			// Use the degrees obtained above and the number of vertices
			// to determine the shape of the contour
			if (vtc == 4 && mincos >= -0.1 && maxcos <= 0.3) {
				setLabel(dst, "RECT", contours[i], getHSV(hsvImg, contours[i]));
				detectedContours.push_back(contours[i]);
			}
		}
		else
		{
			// Detect and label circles
			double area = cv::contourArea(contours[i]);
			cv::Rect r = cv::boundingRect(contours[i]);
			int radius = r.width / 2;

			if (std::abs(1 - ((double)r.width / r.height)) <= 0.3 &&
				std::abs(1 - (area / (CV_PI * std::pow(radius, 2)))) <= 0.3) {
				setLabel(dst, "CIR", contours[i], getHSV(hsvImg, contours[i]));
				detectedContours.push_back(contours[i]);
			}
		}
		drawContours(dst, detectedContours, -1, (255, 0, 0), 3);

		cv::imshow("Detected objects", dst);
	}
}

ObjectDetection::~ObjectDetection()
{
}

/**
* Helper function to display text in the center of a contour
*/
void ObjectDetection::setLabel(cv::Mat& im, const std::string label, std::vector<cv::Point>& contour, Vec3b intensity)
{
	int fontface = cv::FONT_HERSHEY_SIMPLEX;
	double scale = 0.4;
	int thickness = 1;
	int baseline = 0;

	std::string textDisplay = label + " h" + std::to_string(intensity.val[0]) + " s" + std::to_string(intensity.val[1]) + " v" + std::to_string(intensity.val[2]);

	cv::Size text = cv::getTextSize(textDisplay, fontface, scale, thickness, &baseline);
	cv::Rect r = cv::boundingRect(contour);

	cv::Point pt(r.x + ((r.width - text.width) / 2), r.y + ((r.height + text.height) / 2));
	cv::rectangle(im, pt + cv::Point(0, baseline), pt + cv::Point(text.width, -text.height), CV_RGB(255, 255, 255), CV_FILLED);
	cv::putText(im, textDisplay, pt, fontface, scale, CV_RGB(0, 0, 0), thickness, 8);
}

Vec3b ObjectDetection::getHSV(Mat hsvImg, std::vector<Point> contour) {
	/// Get the moments

	Moments moment = moments(contour, false);

	///  Get the mass centers:

	Point2f p = Point2f(moment.m10 / moment.m00, moment.m01 / moment.m00);
	return hsvImg.at<Vec3b>(p);

}

/**
* Helper function to find a cosine of angle between vectors
* from pt0->pt1 and pt0->pt2
*/
double ObjectDetection::angle(cv::Point pt1, cv::Point pt2, cv::Point pt0)
{
	double dx1 = pt1.x - pt0.x;
	double dy1 = pt1.y - pt0.y;
	double dx2 = pt2.x - pt0.x;
	double dy2 = pt2.y - pt0.y;
	return (dx1*dx2 + dy1*dy2) / sqrt((dx1*dx1 + dy1*dy1)*(dx2*dx2 + dy2*dy2) + 1e-10);
}
