	/**
	* @file Morphology_1.cpp
	* @brief Erosion and Dilation sample code
	* @author OpenCV team
	*/


#include <Windows.h>
#include "ObjectDetection.h"
#include "ImageProcessing.h"

	//#include <boost/thread/thread.hpp>

	using namespace cv;

/**
* @function main
*/


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

	//ImageProcessing imageProcess("C:/Users/Michal/Documents/Visual Studio 2015/Projects/vizs_object_detection/Video_27-02-2017_01-49-50");
	ImageProcessing imageProcess("C:/Users/Viktor Dluhoš/Documents/Visual Studio 2015/Projects/Demo_app/Video_27-02-2017_01-49-50");
	//ImageProcessing imageProcess(0);
	imageProcess.doImageProcessing();

#endif
	return 0;
}
