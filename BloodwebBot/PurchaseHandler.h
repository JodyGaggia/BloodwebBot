#pragma once
#include "Node.h"

// Purchases a node in the bloodweb
void PurchaseNode(Node* node, cv::Mat& bloodwebScreenshot, std::vector<Node>& identifiedNodes);
// Continually purchases the cheapest nodes
void PurchaseImmediatelyCheapestNodes(std::vector<Node>& bloodwebNodes, cv::Mat bloodwebScreenshot);
// Return a vector containing all parent nodes
std::vector<Node*> GetNodeTree(Node& leafNode);
// Purchases priority nodes and then continually purchases the cheapest available nodes
void PurchasePriorityNodes(std::vector<Node>& bloodwebNodes, 
							std::vector<std::reference_wrapper<Node>>& priorityNodes, 
							cv::Mat& bloodwebScreenshot);