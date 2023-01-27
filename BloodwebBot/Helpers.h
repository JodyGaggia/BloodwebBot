#pragma once
#include <opencv2/imgproc.hpp>
#include "Node.h"

// Returns a screenshot of the bloodweb
cv::Mat ScreenshotBloodweb(int x, int y, int w, int h);
// Identifies nodes on screen and adds them to a vector
void FillNodesVector(std::vector<Node>& vectorOfNodes, cv::Mat& bloodwebScreenshot);
// Lists all info about a node
void ListNodeInfo(Node node);
