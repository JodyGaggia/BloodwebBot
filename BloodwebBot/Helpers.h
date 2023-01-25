#pragma once
#include <opencv2/imgproc.hpp>
#include "Node.h"

// Returns a screenshot of the bloodweb
cv::Mat ScreenshotBloodweb(int x, int y, int w, int h);
// Returns paths to all files in a directory
std::vector<std::string> ListFiles(std::string path);
// Identifies nodes on screen and adds them to a vector
void FillNodesVector(std::vector<Node>& vectorOfNodes, cv::Mat& bloodwebScreenshot);
// Lists all info about a node
void ListNodeInfo(Node node);
// Creates resources directory if it does not already exist
void PerformFirstTimeSetup();

