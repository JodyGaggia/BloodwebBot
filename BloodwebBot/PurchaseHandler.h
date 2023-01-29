#pragma once
#include "Node.h"

// Purchases priority nodes and then continually purchases the cheapest available nodes
void PurchaseNodes(std::vector<Node>& bloodwebNodes, cv::Mat& bloodwebScreenshot);