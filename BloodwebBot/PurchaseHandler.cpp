#include <iostream>
#include <vector>
#include <Windows.h>
#include "Node.h"
#include "Constants.h"
#include "NodeAnalysis.h"
#include "Helpers.h"

extern bool debug;

void PurchasePriorityNodes(std::vector<Node>& bloodwebNodes, std::vector<std::reference_wrapper<Node>>& priorityNodes, cv::Mat& bloodwebScreenshot);
std::vector<Node*> GetNodeTree(Node& leafNode);
void PurchaseImmediatelyCheapestNodes(std::vector<Node>& bloodwebNodes, cv::Mat bloodwebScreenshot);
void PurchaseNode(Node* node, cv::Mat& bloodwebScreenshot, std::vector<Node>& identifiedNodes);
bool CheckForManualStop();
void SetNodeToPurchased(Node* node);

void PurchasePriorityNodes(std::vector<Node>& bloodwebNodes, std::vector<std::reference_wrapper<Node>>& priorityNodes, cv::Mat& bloodwebScreenshot) {
	// Grab all priority nodes
	for (Node& node : bloodwebNodes) {
		// Ignore node if not priority
		if (!node.GetPriority()) continue;

		std::vector<Node*> nodeTree = GetNodeTree(node);

		// Purchase nodes from base up
		for (int i = nodeTree.size() - 1; i >= 0; i--) {
			if (CheckForManualStop()) return;

			PurchaseNode(nodeTree[i], bloodwebScreenshot, bloodwebNodes);

			// If the entity eats the priority node then we can stop worrying about this chain
			if (node.GetEntityConsumed())
				break;
		}
	}

	// Purchase available nodes until the bloodweb is complete
	PurchaseImmediatelyCheapestNodes(bloodwebNodes, bloodwebScreenshot);
	if (CheckForManualStop()) return;

	// Empty our vectors
	// Note: we use references not pointers so we don't need to deallocate the memory
	bloodwebNodes.clear();
	priorityNodes.clear();

	Sleep(2000);

	FillNodesVector(bloodwebNodes, bloodwebScreenshot);
	PurchasePriorityNodes(bloodwebNodes, priorityNodes, bloodwebScreenshot);

	return;
}

std::vector<Node*> GetNodeTree(Node& leafNode) {
	// Vector of nodes
	std::vector<Node*> nodeTree;

	// Add the leaf node to the vector
	nodeTree.push_back(&leafNode);

	// Add the leaf node's parent to the vector
	Node* parentNode = leafNode.GetParentNodes()[0];
	nodeTree.push_back(parentNode);

	// Add the parent of the parent if need be
	if (!parentNode->GetPurchaseAvailability()) {
		nodeTree.push_back(parentNode->GetParentNodes()[0]);
	}

	return nodeTree;
}

void PurchaseImmediatelyCheapestNodes(std::vector<Node>& bloodwebNodes, cv::Mat bloodwebScreenshot) {
	while (true) {
		int availableAndUnpurchasable = 0;

		for (Node& node : bloodwebNodes) {
			if (CheckForManualStop()) return;

			// Skip this node if it's not currently purchasable
			if (!node.GetPurchaseAvailability()) {
				// If the node hasn't been eaten by the entity yet, it can potentially be purchased in the future
				if (!node.GetEntityConsumed()) availableAndUnpurchasable++;
				continue;
			}

			// Purchase the node...
			PurchaseNode(&node, bloodwebScreenshot, bloodwebNodes);
		}

		// Exit the loop if all nodes have been purchased
		if (availableAndUnpurchasable == 0) return;
	}
}

/**
* Purchases a node on screen
*
* @param[in] location: location of the node on the screen
* @return void
*/
void PurchaseNode(Node* node, cv::Mat& bloodwebScreenshot, std::vector<Node>& identifiedNodes) {
	// We've bought this node now
	SetNodeToPurchased(node);
	
	// Set up mouse input type
	INPUT Input{ 0 };
	Input.type = INPUT_MOUSE;

	// Convert the node's location in the screenshot to a point on the screen
	cv::Point nodeLocation = node->GetLocation() + cv::Point(IMAGE_ORIGIN_X, IMAGE_ORIGIN_Y);

	// Set cursor position to node
	SetCursorPos(nodeLocation.x, nodeLocation.y);

	Sleep(400);

	// Hold left click for 475ms
	Input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
	SendInput(1, &Input, sizeof(INPUT));

	Sleep(475);

	Input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
	SendInput(1, &Input, sizeof(INPUT));

	// Set cursor to top right of screen
	// Stop interference with UI popups
	SetCursorPos(0, 0);

	Sleep(100);

	// FUCK YOU MYSTERY BOXES
	Input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
	SendInput(1, &Input, sizeof(INPUT));
	Sleep(10);
	Input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
	SendInput(1, &Input, sizeof(INPUT));

	Sleep(900);

	// Update nodes taken by entity
	bloodwebScreenshot = ScreenshotBloodweb(IMAGE_ORIGIN_X, IMAGE_ORIGIN_Y, IMAGE_WIDTH, IMAGE_HEIGHT);
	UpdateEntityNodes(bloodwebScreenshot, identifiedNodes);

	std::cout << "PURCHASE:" << std::endl;
	std::cout << "	Ring: " << node->GetRing() << std::endl;
	std::cout << "	Rarity: " << node->GetRarity() << std::endl;
	std::cout << "	Angle: " << node->GetAngleFromCenter() << std::endl << std::endl;
}

bool CheckForManualStop() {
	// If the user holds down 'p' the program should stop
	if (GetKeyState(0x50) & 0x8000) {
		std::cout << "Stopping..." << std::endl << std::endl;
		cv::destroyAllWindows();
		return true;
	}

	return false;
}

void SetNodeToPurchased(Node* node) {
	// This node is no longer accessible or purchasable
	node->SetEntityConsumed(true);
	node->SetPurchaseAvailability(false);

	// Child nodes are now purchasable
	for (Node* child : node->GetChildNodes()) {
		child->SetPurchaseAvailability(true);
	}
}