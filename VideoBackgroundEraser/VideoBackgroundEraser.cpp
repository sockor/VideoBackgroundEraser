// VideoBackgroundEraser.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream> // for standard I/O
#include <string>   // for strings
#include <iomanip>  // for controlling float print precision
#include <sstream>  // string to number conversion

#include <opencv2/core.hpp>     // Basic OpenCV structures (cv::Mat, Scalar)
#include <opencv2/videoio.hpp>
#include <opencv2/opencv.hpp>

#include "FrameHistory.h"

using namespace std;
using namespace cv;

int main(int argc, char *argv[])
{
	if (argc < 2)
	{
		cout << "Not enough parameters" << argc << endl;
		return -1;
	}
	const string sourceReference = argv[1];
	VideoCapture cap(sourceReference, CAP_ANY);
	if (!cap.isOpened())
	{
		cout << "Could not open reference " << sourceReference << endl;
		return -1;
	}
	int frameNum = -1;          // Frame counter
	int totalFrameCount = (int)cap.get(CAP_PROP_FRAME_COUNT);

	FrameHistory history;
	if (argc >= 3)
	{
		history.SetHistoryMax(std::stoi(argv[2]));
	}
	if (argc >= 4)
	{
		history.SetThreshold(std::stoi(argv[3]));
	}

	string filename("out_");
	string extension(".avi");
	int fourcc = VideoWriter::fourcc('M', 'J', 'P', 'G');
	Size videoSize = Size(cap.get(CAP_PROP_FRAME_WIDTH), cap.get(CAP_PROP_FRAME_HEIGHT));
	VideoWriter videoOut("out.avi", fourcc, cap.get(CAP_PROP_FPS), videoSize);

	for (;;) //Show the image captured in the window and repeat
	{
		Mat frame;
		cap.read(frame);
		if (frame.empty())
		{
			break;
		}
		++frameNum;
		if (frameNum % 10 == 0)
		{
			cout << "Frame: " << frameNum << " / " << totalFrameCount << endl;
		}

		history.TryRemoveBackground(frame);

		videoOut.write(frame);

		imshow("Frame", frame);

		char c = (char)waitKey(1);
		if (c == 27)
		{
			break;
		}
		else if (c == 32)
		{
			while ((char)waitKey(0) != 32);
		}
	}
	videoOut.release();
	return 0;
}

