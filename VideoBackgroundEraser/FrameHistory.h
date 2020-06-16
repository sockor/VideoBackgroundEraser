#pragma once
#include <opencv2/videoio.hpp>

class FrameHistory
{
private:
	int historyMax = 3;
	int identityThreshold = 60;
	std::vector<cv::Mat*> frameHistory;
	int nextHistoryIndex;

public:
	FrameHistory();
	~FrameHistory();
	void SetHistoryMax(int max);
	void SetThreshold(int threshold);
	void TryRemoveBackground(cv::Mat& frame);
	const cv::Mat& GetLastestHistory();

private:
	bool IsHistoryFull() { return frameHistory.size() >= historyMax; }
	void MoveForwardNextHistoryIndex();
	void RecordFrame(cv::Mat* frame);
	void RemoveBackgroundCore(cv::Mat& frame);
	bool IsIdenticalFrame(cv::Mat & frame, int col, int row);
	int GetColorDiff(const cv::Vec3b& colorA, const cv::Vec3b& colorB);
};

