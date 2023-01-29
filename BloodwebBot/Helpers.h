#pragma once
#include <opencv2/imgproc.hpp>
#include "Node.h"

// Returns a screenshot of the bloodweb
cv::Mat ScreenshotBloodweb(int x, int y, int w, int h);
// Populates a vector with all nodes found in the current bloodweb on screen and also fills a cv::Mat with a screenshot of the bloodweb
void FillNodesVector(std::vector<Node>& vectorOfNodes, cv::Mat& bloodwebScreenshot);
// Lists all info about a node
void ListNodeInfo(Node node);
