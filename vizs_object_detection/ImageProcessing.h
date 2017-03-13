#pragma once
#pragma warning(disable : 4996)
#include "ObjectDetection.h"
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/opencv.hpp"

#include <time.h>
#include <direct.h>
#include <exception>

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

	Mat getFrameFromLifeStream(int source) {

		Mat frame;
		
		if (openCamera(source)) {
			cap >> frame;
		}

		return frame;
	}

private:

	std::string img_name;
	std::string path;
	std::vector<Mat> frames;
	VideoCapture cap;

	std::string get_string_dateAndTime()
	{
		time_t rawtime;
		struct tm * timeinfo;
		char cas[40];

		time(&rawtime);
		try {
			timeinfo = localtime(&rawtime);
			strftime(cas, 40, "%d-%m-%Y_%I-%M-%S", timeinfo); //magic format
		}
		catch (std::exception& e) {

		}
		std::string str(cas);
		return str;
	}

	bool openCamera(int source) {
		 
		static bool cameraOn = false;
		
		if (!cameraOn) {
			// open the default camera, use something different from 0 otherwise;
			// Check VideoCapture documentation.
			if (!cap.open(source)) {
				printf("WEBCAM NOT FOUND!\n");
				return false;
			}
			else {
				cameraOn = true;
				printf("WEBCAM WAS OPENED!\n");
				return true;
			}
		}
		return true;
	}


};

class ImageProcessing : public Video {
public:

	const static int LIFESTREAM = 1;
	const static int FILE = 2;

	ImageProcessing(std::vector <Mat> image) {
		this->imageVector = image;

	}

	ImageProcessing(std::string path) : Video(path) {

		type = FILE;
	}

	ImageProcessing(int videoSource) : Video() {

		type = LIFESTREAM;
		this->videoSource = videoSource;
	}

	void doImageProcessing() {

		int i = 1;
		Mat image;
		while (1) {

			switch (type) {
			case LIFESTREAM:
				image = getFrameFromLifeStream(videoSource);
				break;
			case FILE:
				image = getFrame(i++);
				break;
			default:
				image = imageVector[i++];
			}

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
	std::vector <Mat> imageVector;
	Mat smoothImage;
	Mat grayImage;
	Mat edgeDetectionImage;
	Mat OTSUImage;

	int type;
	int videoSource;

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
