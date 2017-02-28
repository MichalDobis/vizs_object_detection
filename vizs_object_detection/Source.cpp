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

/// Global variables
Mat src, erosion_dst, dilation_dst;

int erosion_elem = 0;
int erosion_size = 0;
int dilation_elem = 0;
int dilation_size = 0;
int const max_elem = 2;
int const max_kernel_size = 21;

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


class ImageProcessing: public Video{
public:
	ImageProcessing(std::vector <Mat> image)  {
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
			//waitKey();
			GaussianBlur(image, smoothImage, cv::Size(5, 5), 1.5);
			cvtColor(smoothImage, grayImage, CV_BGR2GRAY);
			Canny(grayImage, edgeDetectionImage, 100, 100);
			threshold(grayImage, OTSUImage, 0, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);

			imshow("VIDEO PLAY", image);		//show normal image
													//imshow("smooth video", smoothImage);	//show smooth image
			imshow("Edge decetion CANNY", edgeDetectionImage);	//show edge detection
			imshow("OTSU Treshold", OTSUImage);
			thresh_callback(0, 0);

		}

	}

private:
	std::vector <Mat> image;
	Mat smoothImage;
	Mat grayImage;
	Mat edgeDetectionImage;
	Mat OTSUImage;

	void thresh_callback(int, void*)
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
		imshow("Contours from OTSU", drawing);
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