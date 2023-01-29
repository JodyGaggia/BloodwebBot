#include <iostream>
#include <vector>
#include <Windows.h>
#include "Node.h"
#include "Constants.h"
#include "NodeAnalysis.h"
#include "Helpers.h"

extern bool debug;

void PurchaseNodes(std::vector<Node>& bloodwebNodes, cv::Mat& bloodwebScreenshot);
std::vector<Node*> GetNodeTree(Node& leafNode);
void PurchaseImmediatelyCheapestNodes(std::vector<Node>& bloodwebNodes, cv::Mat bloodwebScreenshot);
void PurchaseNode(Node* node, cv::Mat& bloodwebScreenshot, std::vector<Node>& identifiedNodes);
bool CheckForManualStop();
void SetNodeToPurchased(Node* node);
void PrestigeCharacter();

/*
* Purchases all nodes in the bloodweb, beginning with those which are priority nodes
* 
* @param[in] bloodwebNodes: A vector which can store detected nodes in the bloodweb
* @param[in] bloodwebScreenshot: A material which can store a screenshot of the bloodweb
* @return void
*/
void PurchaseNodes(std::vector<Node>& bloodwebNodes, cv::Mat& bloodwebScreenshot) {
	// Empty our nodes vectors so they're ready to be filled again
	bloodwebNodes.clear();

	// Fill said vectors...
	FillNodesVector(bloodwebNodes, bloodwebScreenshot);
	
	if (bloodwebNodes.size() < 3) {
		PrestigeCharacter();
		PurchaseNodes(bloodwebNodes, bloodwebScreenshot);
	}

	// Grab all priority nodes
	for (Node& node : bloodwebNodes) {
		// Ignore node if not priority
		if (!node.GetPriority()) continue;

		std::vector<Node*> nodeTree = GetNodeTree(node);

		// Purchase nodes from base up
		for (int i = nodeTree.size() - 1; i >= 0; i--) {
			if (CheckForManualStop() || bloodpoints <= 0) return;

			PurchaseNode(nodeTree[i], bloodwebScreenshot, bloodwebNodes);

			// If the entity eats the priority node then we can stop worrying about this chain
			if (node.GetEntityConsumed())
				break;
		}
	}

	// Purchase available nodes until the bloodweb is complete
	PurchaseImmediatelyCheapestNodes(bloodwebNodes, bloodwebScreenshot);
	if (CheckForManualStop() || bloodpoints <= 0) return;

	std::cout << "Bloodweb complete." << std::endl;
	Sleep(2000);

	PurchaseNodes(bloodwebNodes, bloodwebScreenshot);
	
	return;
}

/*
* Returns a chain from the input node to the base of the bloodweb
* 
* returnVector[0] = input node
* returnVector[1] = input node's parent
* returnVector[2] = parent of input node's parent
* 
* @param[in] leafNode: Node to find this chain from
* @return std::vector<Node*>
*/
std::vector<Node*> GetNodeTree(Node& leafNode) {
	// Vector of nodes
	std::vector<Node*> nodeTree;

	// Add the leaf node to the vector
	nodeTree.push_back(&leafNode);

	// Return immediately if in the first ring
	if (leafNode.GetRing() == 1) return nodeTree;

	// Add the leaf node's parent to the vector
	Node* parentNode = leafNode.GetParentNodes()[0];
	nodeTree.push_back(parentNode);

	// Add the parent of the parent if need be
	if (!parentNode->GetPurchaseAvailability()) {
		nodeTree.push_back(parentNode->GetParentNodes()[0]);
	}

	return nodeTree;
}

// This really isn't efficient but oh well
void PurchaseImmediatelyCheapestNodes(std::vector<Node>& bloodwebNodes, cv::Mat bloodwebScreenshot) {
	while (true) {
		for (int i = 0; i < bloodwebNodes.size(); i++) {
			if (CheckForManualStop() || bloodpoints <= 0) return; // Stop if user says so

			// Vector is sorted by price so just buy the first purchasable node
			if (bloodwebNodes[i].GetPurchaseAvailability()) {
				PurchaseNode(&bloodwebNodes[i], bloodwebScreenshot, bloodwebNodes);
				break;
			}
			
			// If we have iterated through all nodes and not purchased any then return
			if (i == bloodwebNodes.size() - 1) return;
		}
	}
}

/**
* Purchases a node on screen
*
* @param[in] location: location of the node on the screen
* @return void
*/
void PurchaseNode(Node* node, cv::Mat& bloodwebScreenshot, std::vector<Node>& identifiedNodes) {
	std::cout << "PURCHASE:" << std::endl;
	std::cout << "	Ring: " << node->GetRing() << std::endl;
	std::cout << "	Rarity: " << node->GetRarity() << std::endl;
	std::cout << "	Angle: " << node->GetAngleFromCenter() << std::endl << std::endl;

	// We've bought this node now
	SetNodeToPurchased(node);
	bloodpoints -= rarityToCost.at(node->GetRarity());

	// Set up mouse input type
	INPUT Input{ 0 };
	Input.type = INPUT_MOUSE;

	// Convert the node's location in the screenshot to a point on the screen
	cv::Point nodeLocation = node->GetLocation() + cv::Point(IMAGE_ORIGIN_X, IMAGE_ORIGIN_Y);

	// Set cursor position to node
	SetCursorPos(nodeLocation.x, nodeLocation.y);

	Sleep(100);

	// Hold left click for 475ms
	Input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
	SendInput(1, &Input, sizeof(INPUT));

	Sleep(475);

	Input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
	SendInput(1, &Input, sizeof(INPUT));

	//SetCursorPos(nodeLocation.x - 150, nodeLocation.y + 80);

	Sleep(100);

	for (int i = 0; i < 8; i++) {
		// FUCK YOU MYSTERY BOXES
		Input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
		SendInput(1, &Input, sizeof(INPUT));
		Sleep(20);
		Input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
		SendInput(1, &Input, sizeof(INPUT));

		Sleep(100);
	}

	// Set cursor to top left of screen
	// Stop interference with UI popups
	SetCursorPos(10, 10);

	Sleep(100);

	// Update nodes taken by entity
	bloodwebScreenshot = ScreenshotBloodweb(IMAGE_ORIGIN_X, IMAGE_ORIGIN_Y, IMAGE_WIDTH, IMAGE_HEIGHT);
	UpdateEntityNodes(bloodwebScreenshot, identifiedNodes);
}

/*
* Prestiges the current character
*/
void PrestigeCharacter() {
	// Set up mouse input type
	INPUT Input{ 0 };
	Input.type = INPUT_MOUSE;

	// Convert the node's location in the screenshot to a point on the screen
	cv::Point nodeLocation = cv::Point(BLOODWEB_CENTER_X, BLOODWEB_CENTER_Y) + cv::Point(IMAGE_ORIGIN_X, IMAGE_ORIGIN_Y);

	SetCursorPos(nodeLocation.x, nodeLocation.y);

	Sleep(100);

	// Hold left click for 1000ms
	Input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
	SendInput(1, &Input, sizeof(INPUT));

	Sleep(2500);

	Input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
	SendInput(1, &Input, sizeof(INPUT));

	SetCursorPos(10, 10);

	Sleep(4000);
}

/*
* Checks if the user is holding down 'p'. This is used to halt the program.
*/
bool CheckForManualStop() {
	// If the user holds down 'p' the program should stop
	if (GetKeyState(0x50) & 0x8000) {
		std::cout << "Stopping..." << std::endl << std::endl;
		cv::destroyAllWindows();
		return true;
	}

	return false;
}

/*
* Helper function for PurchaseNode. Sets the input node as entity consumed (ie. unavailable) and purchased. Sets children to be purchasable if not entity consumed.
* 
* @param[in] node: The node to set to purchased and for which its children should be set to purchasable
* @return void
*/
void SetNodeToPurchased(Node* node) {
	// This node is no longer accessible or purchasable
	node->SetEntityConsumed(true);
	node->SetPurchaseAvailability(false);

	// Child nodes are now purchasable
	for (Node* child : node->GetChildNodes()) {
		if(!child->GetEntityConsumed())
			child->SetPurchaseAvailability(true);
	}
}