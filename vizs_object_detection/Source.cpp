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

/** Function Headers */
void Erosion(int, void*);
void Dilation(int, void*);

/**
* @function main
*/
using namespace cv;

class Video {
public:
	Video(std::string path) {

		this->path = path;
		img_name = "img_";
	}

	bool saveImg(Mat frame) {
		static int counter = 0;

		return imwrite(path + "/" + img_name + std::to_string(counter++) + ".jpg", frame);
	}

	std::vector<Mat> getLoadedFrames(int first_frame, int last_frame) {
		loadImg(first_frame, last_frame);
		return frames;
	}

private:

	std::string img_name;
	std::string path;

	std::vector<Mat> frames;
	
	void loadImg(int first_frame, int last_frame) {
		frames.clear();
		
		for (int counter = first_frame; counter <= last_frame; counter++) {
			frames.push_back(imread(path + "/" + img_name + std::to_string(counter) + ".jpg", IMREAD_COLOR));
			printf("path file: %s\n", std::string(path + "/" + img_name + std::to_string(counter) + ".jpg").c_str());
		}
	}
};


//#define CAPTURE_VIDEO

int main(int argc, char** argv)
{

	Video myVideo("demo");

#ifdef CAPTURE_VIDEO

	VideoCapture cap;
	// open the default camera, use something different from 0 otherwise;
	// Check VideoCapture documentation.
	if (!cap.open(0))
		return 0;

	for (;;)
	{
		Mat frame;
		cap >> frame;
		if (frame.empty())  break; // end of video stream
		imshow("this is you, smile! :)", frame);

		myVideo.saveImg(frame);
		if (waitKey(10) == 27) break; // stop capturing by pressing ESC 
	}
	// the camera will be closed automatically upon exit
	// cap.close();
#else 
	std::vector<Mat> frames = myVideo.getLoadedFrames(0,100);
	
	printf("size frames %d", frames.size());
	for (int i = 0; i < frames.size(); i++) {
		if (frames[i].empty())  printf("error");
		imshow("loaded frame", frames[i]);
		cv::waitKey();

		//Sleep(1000);
		//boost::this_thread::sleep(boost::posix_time::milliseconds(1000));
	}

	Mat test = imread("image.jpg", CV_LOAD_IMAGE_COLOR);
	if (test.empty()) printf("test file is emptyS");

	//namedWindow("bla", CV_WINDOW_AUTOSIZE);
	//imshow("bla", test);
	//cv::waitKey(0);

	Sleep(1000);
#endif
	return 0;
}

//![erosion]
/**
* @function Erosion
*/
void Erosion(int, void*)
{
	int erosion_type = 0;
	if (erosion_elem == 0) { erosion_type = MORPH_RECT; }
	else if (erosion_elem == 1) { erosion_type = MORPH_CROSS; }
	else if (erosion_elem == 2) { erosion_type = MORPH_ELLIPSE; }

	//![kernel]
	Mat element = getStructuringElement(erosion_type,
		Size(2 * erosion_size + 1, 2 * erosion_size + 1),
		Point(erosion_size, erosion_size));
	//![kernel]
	
	/// Apply the erosion operation
	erode(src, erosion_dst, element);
	imshow("Erosion Demo", erosion_dst);
}
//![erosion]

//![dilation]
/**
* @function Dilation
*/
void Dilation(int, void*)
{
	int dilation_type = 0;
	if (dilation_elem == 0) { dilation_type = MORPH_RECT; }
	else if (dilation_elem == 1) { dilation_type = MORPH_CROSS; }
	else if (dilation_elem == 2) { dilation_type = MORPH_ELLIPSE; }

	Mat element = getStructuringElement(dilation_type,
		Size(2 * dilation_size + 1, 2 * dilation_size + 1),
		Point(dilation_size, dilation_size));

	/// Apply the dilation operation
	dilate(src, dilation_dst, element);
	imshow("Dilation Demo", dilation_dst);
}
//![dilation]
