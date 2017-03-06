	/**
	* @file Morphology_1.cpp
	* @brief Erosion and Dilation sample code
	* @author OpenCV team
	*/

#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/opencv.hpp"
#include <Windows.h>
#include <time.h>
#include <direct.h>
	//#include <boost/thread/thread.hpp>

	using namespace cv;

/**
* @function main
*/
using namespace cv;

class Video {
public:
	Video() {
		img_name = "img_";
	}

	Video(std::string path) {

		this->path = path;
		img_name = "img_";
	}

	bool createDirectory(std::string dir) {

		path = dir + get_string_dateAndTime();
		_mkdir(path.c_str());
	}

	bool saveImg(Mat frame) {

		static int counter = 1;
		return imwrite(path + "/" + img_name + std::to_string(counter++) + ".jpg", frame);
	}

	std::vector<Mat> getLoadedFrames(int first_frame, int last_frame) {

		frames.clear();

		for (int counter = first_frame; counter <= last_frame; counter++) {
			frames.push_back(getFrame(counter));
		}
		return frames;
	}

protected:
	Mat getFrame(int index) {
		printf((path + "/" + img_name + std::to_string(index) + ".jpg\n").c_str());
		return imread(path + "/" + img_name + std::to_string(index) + ".jpg", IMREAD_COLOR);
	}

private:

	std::string img_name;
	std::string path;
	std::vector<Mat> frames;

	std::string get_string_dateAndTime()
	{
		time_t rawtime;
		struct tm * timeinfo;
		char cas[40];

		time(&rawtime);
		timeinfo = localtime(&rawtime);
		strftime(cas, 40, "%d-%m-%Y_%I-%M-%S", timeinfo); //magic format
		std::string str(cas);
		return str;
	}
};

class ObjectDetection {
public:
	ObjectDetection(std::vector<std::vector<Point> > contours, Mat src) {

		std::vector<cv::Point> approx;
		cv::Mat dst = src.clone();
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
				setLabel(dst, "TRI", contours[i]);    // Triangles
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
					setLabel(dst, "RECT", contours[i]);
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
					setLabel(dst, "CIR", contours[i]);
					detectedContours.push_back(contours[i]);
				}
			}
			drawContours(dst, detectedContours, -1, (255, 0, 0), 3);

			cv::imshow("Detected objects", dst);
		}


	}
private:
	/**
	* Helper function to find a cosine of angle between vectors
	* from pt0->pt1 and pt0->pt2
	*/
	static double angle(cv::Point pt1, cv::Point pt2, cv::Point pt0)
	{
		double dx1 = pt1.x - pt0.x;
		double dy1 = pt1.y - pt0.y;
		double dx2 = pt2.x - pt0.x;
		double dy2 = pt2.y - pt0.y;
		return (dx1*dx2 + dy1*dy2) / sqrt((dx1*dx1 + dy1*dy1)*(dx2*dx2 + dy2*dy2) + 1e-10);
	}

	/**
	* Helper function to display text in the center of a contour
	*/
	void setLabel(cv::Mat& im, const std::string label, std::vector<cv::Point>& contour)
	{
		int fontface = cv::FONT_HERSHEY_SIMPLEX;
		double scale = 0.4;
		int thickness = 1;
		int baseline = 0;

		cv::Size text = cv::getTextSize(label, fontface, scale, thickness, &baseline);
		cv::Rect r = cv::boundingRect(contour);

		cv::Point pt(r.x + ((r.width - text.width) / 2), r.y + ((r.height + text.height) / 2));
		cv::rectangle(im, pt + cv::Point(0, baseline), pt + cv::Point(text.width, -text.height), CV_RGB(255, 255, 255), CV_FILLED);
		cv::putText(im, label, pt, fontface, scale, CV_RGB(0, 0, 0), thickness, 8);
	}
};

class ImageProcessing : public Video {
public:
	ImageProcessing(std::vector <Mat> image) {
		this->image = image;

	}

	ImageProcessing(std::string path) : Video(path) {

	}

	void doImageProcessing() {

		int i = 1;
		while (1) {

			Mat image = getFrame(i++);
			if (image.empty()) {
				break;
			}
			waitKey(50);
			GaussianBlur(image, smoothImage, cv::Size(5, 5), 1.5);
			cvtColor(smoothImage, grayImage, CV_BGR2GRAY);
			Canny(grayImage, edgeDetectionImage, 100, 100);
			threshold(grayImage, OTSUImage, 0, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);

			//imshow("VIDEO PLAY", image);		//show normal image
			//imshow("VIDEO PLAY with blur img", smoothImage);	//show smooth image
																//imshow("Edge decetion with CANNY", edgeDetectionImage);	//show edge detection
			//imshow("OTSU Treshold", OTSUImage);

			ObjectDetection obj(thresh_callback(0, 0), smoothImage);
		}

	}

private:
	std::vector <Mat> image;
	Mat smoothImage;
	Mat grayImage;
	Mat edgeDetectionImage;
	Mat OTSUImage;

	std::vector<std::vector<Point> > thresh_callback(int, void*)
	{
		Mat treshold_output;

		std::vector<std::vector<Point> > contours;
		std::vector<Vec4i> hierarchy;

		/// Detect edges using canny
		//Canny(gray_image, treshold_output, thresh, thresh * 2, 3);
		threshold(grayImage, treshold_output, 0, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);

		/// Find contours
		findContours(treshold_output, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0));

		/// Draw contours
		Mat drawing = Mat::zeros(treshold_output.size(), CV_8UC3);
		for (int i = 0; i< contours.size(); i++)
		{
			Scalar color = Scalar(0, 0, 255);
			drawContours(drawing, contours, i, color, 2, 8, hierarchy, 0, Point());
		}

		/// Show in a window
		//namedWindow("Contours", WINDOW_AUTOSIZE);
		imshow("Contours with OTSU threshold", drawing);

		return contours;
	}
};

//#define CAPTURE_VIDEO

int main(int argc, char** argv)
{

#ifdef CAPTURE_VIDEO

	Video myVideo("demo");
	VideoCapture cap;
	// open the default camera, use something different from 0 otherwise;
	// Check VideoCapture documentation.
	if (!cap.open(0))
		printf("WEBCAM NOT FOUND!\n");
	else
		printf("WEBCAM WAS OPENED!\n");

	video.createDirectory("../Video_");

	for (;;)
	{
		Mat frame;
		cap >> frame;
		if (frame.empty())  break; // end of video stream
		imshow("Video capture", frame);

		myVideo.saveImg(frame);
		if (waitKey(10) == 27) {
			cvDestroyWindow("Video capture");
			break; // stop capturing by pressing ESC 
		}
	}
	// the camera will be closed automatically upon exit
	// cap.close();
#else 
	//Video myVideo("C:/Users/Michal/Documents/Visual Studio 2015/Projects/vizs_object_detection/Video_27-02-2017_01-49-50");
	//Video myVideo("demo");
	//std::vector<Mat> frames = myVideo.getLoadedFrames(1,100);

	//	printf("\nsize frames %d\n", frames.size());

	/*for (int i = 0; i < frames.size(); i++) {
	if (frames[i].empty())  printf("error");
	imshow("loaded frame", frames[i]);
	cv::waitKey();

	//Sleep(1000);
	//boost::this_thread::sleep(boost::posix_time::milliseconds(1000));
	}*/

	ImageProcessing imageProcess("C:/Users/Michal/Documents/Visual Studio 2015/Projects/vizs_object_detection/Video_27-02-2017_01-49-50");
	imageProcess.doImageProcessing();

#endif
	return 0;
}
