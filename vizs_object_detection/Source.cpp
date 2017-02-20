/**
* @file Morphology_1.cpp
* @brief Erosion and Dilation sample code
* @author OpenCV team
*/

#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/opencv.hpp"

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
int main(int argc, char** argv)
{
	VideoCapture cap;
	// open the default camera, use something different from 0 otherwise;
	// Check VideoCapture documentation.
	if (!cap.open(2))
		return 0;
	for (;;)
	{
		Mat frame;
		cap >> frame;
		if (frame.empty())  break; // end of video stream
		imshow("this is you, smile! :)", frame);
		//printf("saving image");
		imwrite("image.jpg", frame);
		if (waitKey(10) == 27) break; // stop capturing by pressing ESC 
	}
	// the camera will be closed automatically upon exit
	// cap.close();
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
