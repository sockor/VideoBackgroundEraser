#include "pch.h"
#include "FrameHistory.h"
#include <iostream>
#include <omp.h>


FrameHistory::FrameHistory()
{
	nextHistoryIndex = 0;
}
FrameHistory::~FrameHistory()
{
	for (const cv::Mat* history : frameHistory)
	{
		delete(history);
	}
}

void FrameHistory::SetHistoryMax(int max)
{
	historyMax = max;
	std::cout << historyMax << std::endl;
}

void FrameHistory::SetThreshold(int threshold)
{
	identityThreshold = threshold;
	std::cout << identityThreshold << std::endl;
}


void FrameHistory::TryRemoveBackground(cv::Mat & frame)
{
	cv::Mat* frameBackup = new cv::Mat(frame.clone());
	RemoveBackgroundCore(frame);
	RecordFrame(frameBackup);
}

const cv::Mat & FrameHistory::GetLastestHistory()
{
	// TODO: insert return statement here
	int index = nextHistoryIndex - 1;
	if (index < 0)
	{
		index = historyMax - 1;
	}
	return *frameHistory[index];
}

void FrameHistory::RecordFrame(cv::Mat* frame)
{
	if (!IsHistoryFull())
	{
		frameHistory.push_back(frame);
	}
	else
	{
		delete frameHistory[nextHistoryIndex];
		frameHistory[nextHistoryIndex] = frame;
	}

	MoveForwardNextHistoryIndex();
}
void FrameHistory::RemoveBackgroundCore(cv::Mat & frame)
{
	if(IsHistoryFull())
	{
		omp_set_num_threads(4);
		#pragma omp parallel for
		for (int i = 0; i < frame.rows; i++)
		{
			for (int j = 0; j < frame.cols; j++)
			{
				if (IsIdenticalFrame(frame, i, j))
				{
					cv::Vec3b& currentColor = frame.at<cv::Vec3b>(i, j);
					currentColor.val[0] = 0x7f;
					currentColor.val[1] = 0xff;
					currentColor.val[2] = 0x00;
				}
			}
		}
	}
}

bool FrameHistory::IsIdenticalFrame(cv::Mat & frame, int col, int row)
{
	int diff = 0;
	cv::Vec3b& currentColor = frame.at<cv::Vec3b>(col, row);
	for (const cv::Mat* history : frameHistory)
	{
		const cv::Vec3b& historyColor = history->at<cv::Vec3b>(col, row);
		diff += GetColorDiff(historyColor, currentColor);
	}
	if (diff > identityThreshold)
	{
		return false;
	}
	return true;
}

int FrameHistory::GetColorDiff(const cv::Vec3b & colorA, const cv::Vec3b & colorB)
{
	int diff = 0;

	for (int i = 0; i < 3; i++)
	{
		diff += std::abs((int)colorA.val[i] - (int)colorB.val[i]);
	}
	return diff;
}

void FrameHistory::MoveForwardNextHistoryIndex()
{
	nextHistoryIndex++;
	if (nextHistoryIndex >= historyMax)
	{
		nextHistoryIndex = 0;
	}
}
