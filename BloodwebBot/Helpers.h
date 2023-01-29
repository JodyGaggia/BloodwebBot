#pragma once
#include <opencv2/imgproc.hpp>
#include "Node.h"

/**
* Screenshots the region of the screen in which the bloodweb appears
*
* @param[in] x,y: top left coordinates of area to be screenshotted
* @param[in] w: width of area to be screenshotted
* @param[in] h: height of area to be screenshotted
* @return cv::Mat of screenshotted bloodweb
*/
cv::Mat ScreenshotBloodweb(int x, int y, int w, int h);

/**
* Finds all properties about all nodes in the bloodweb
*
* 1. Detect locations of nodes
* 2. Convert locations to node objects
* 3. Determine rarity of each node
* 4. Set priority of each node
* 5. Set availability of all nodes
* 6. Find parent/child nodes of each node
*
* @param[out] vector of fully classified nodes
*/
void FillNodesVector(std::vector<Node>& vectorOfNodes, cv::Mat& bloodwebScreenshot);

/**
* Prints all information about a given node to the console
*
* @param[in] Node to list info about
*/
void ListNodeInfo(Node node);
