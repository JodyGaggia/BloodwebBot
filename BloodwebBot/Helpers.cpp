#include <Windows.h>
#include <iostream>
#include <string>
#include <vector>
#include <opencv2/imgproc.hpp>
#include <opencv2/world.hpp>
#include <opencv2/features2d/features2d.hpp>
#include "Node.h"
#include "NodeAnalysis.h"
#include "Constants.h"

extern bool debug;

cv::Mat ScreenshotBloodweb(int x, int y, int w, int h);
void FillNodesVector(std::vector<Node>& vectorOfNodes, cv::Mat& bloodwebScreenshot);
void ListNodeInfo(Node node);

/**
* Screenshots the region of the screen in which the bloodweb appears
*
* @param[in] x,y: top left coordinates of area to be screenshotted
* @param[in] w: width of area to be screenshotted
* @param[in] h: height of area to be screenshotted
* @return cv::Mat of screenshotted bloodweb
*/
cv::Mat ScreenshotBloodweb(int x, int y, int w, int h) {
	HDC hdcSource = GetDC(NULL);
	HDC hdcMemory = CreateCompatibleDC(hdcSource);

	HBITMAP hBitmap = CreateCompatibleBitmap(hdcSource, w, h);

	SelectObject(hdcMemory, hBitmap);
	BitBlt(hdcMemory, 0, 0, w, h, hdcSource, x, y, SRCCOPY);

	BITMAPINFOHEADER bi;
	bi.biSize = sizeof(BITMAPINFOHEADER);
	bi.biWidth = w;
	bi.biHeight = -h;
	bi.biPlanes = 1;
	bi.biBitCount = 32;
	bi.biCompression = BI_RGB;
	bi.biSizeImage = 0;
	bi.biXPelsPerMeter = 1;
	bi.biYPelsPerMeter = 2;
	bi.biClrUsed = 3;
	bi.biClrImportant = 4;

	cv::Mat mat = cv::Mat(h, w, CV_8UC4);
	GetDIBits(hdcMemory, hBitmap, 0, h, mat.data, (BITMAPINFO*)&bi, DIB_RGB_COLORS);

	DeleteDC(hdcSource);
	DeleteDC(hdcMemory);
	HPALETTE hpal = NULL;

	return mat;
}

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
void FillNodesVector(std::vector<Node>& vectorOfNodes, cv::Mat& bloodwebScreenshot) {
	cv::Mat imgBlur, imgCanny, imgEdit;

	// Screenshot bloodweb + processing
	bloodwebScreenshot = ScreenshotBloodweb(IMAGE_ORIGIN_X, IMAGE_ORIGIN_Y, IMAGE_WIDTH, IMAGE_HEIGHT);

	cv::blur(bloodwebScreenshot, imgBlur, cv::Size(2, 2));
	cv::Canny(imgBlur, imgCanny, 80, 120); // 60, 150

	imgEdit = bloodwebScreenshot.clone();

	// Grab locations of nodes
	std::vector<cv::Vec3f> nodeLocations = GetNodeLocations(imgCanny);

	// DEBUG: Draw circles around detected nodes
	for (cv::Vec3f node : nodeLocations) {
		cv::circle(imgEdit, cv::Point(node[0], node[1]), node[2], cv::Scalar(0, 255, 0), 3);
	}

	// Classify each detected node
	for (int i = 0; i < nodeLocations.size(); i++) {
		// Convert location to node object
		Node currentNode = ConvertLocationToNode(nodeLocations[i]);

		if (currentNode.GetRing() == 1)
			currentNode.SetPurchaseAvailability(true);

		// Set node rarity
		SetNodeRarity(bloodwebScreenshot, currentNode);

		// Set node priority
		SetNodePriority(bloodwebScreenshot, currentNode);

		vectorOfNodes.push_back(currentNode);
	}

	UpdateEntityNodes(bloodwebScreenshot, vectorOfNodes);
	// Sort in order of rarity
	std::sort(vectorOfNodes.begin(), vectorOfNodes.end(), [](Node a, Node b) { return a.GetRarity() < b.GetRarity(); });

	// Find all node dependencies
	for (int i = 0; i < vectorOfNodes.size(); i++) {
		Node& node = vectorOfNodes[i];

		std::vector<std::reference_wrapper<Node>> dependencies = FindPotentialDependencyNodes(node, vectorOfNodes);
		DetermineDependency(node, dependencies, imgCanny);
	}

	// Draw lines to show connections from parent node to child node and vice-versa
	for (int i = 0; i < vectorOfNodes.size(); i++) {
		Node& node = vectorOfNodes[i];
		for (Node* childNode : node.GetChildNodes()) {
			if (childNode != nullptr) {
				cv::line(imgEdit, node.GetLocation(), childNode->GetLocation(), cv::Scalar(0, 0, 255), 3);
			}
		}
	}

	if (debug) {
		// List info about all nodes
		for (int i = 0; i < vectorOfNodes.size(); i++) {
			ListNodeInfo(vectorOfNodes[i]);
		}

		cv::imshow("Bloodweb", bloodwebScreenshot);
		cv::imshow("Bloodweb, Canny", imgCanny);
		cv::imshow("Bloodweb, Detected", imgEdit);
		cv::waitKey(0);
	}
}

/**
* Prints all information about a given node to the console
*
* @param[in] Node to list info about
*/
void ListNodeInfo(Node node) {
	std::cout << "Node location: " << node.GetLocation() << std::endl;
	std::cout << "	Distance from center: " << node.GetDistanceFromCenter() << std::endl;
	std::cout << "	Angle from center: " << node.GetAngleFromCenter() << std::endl;
	std::cout << "Ring: " << node.GetRing() << std::endl;
	std::cout << "Rarity: " << node.GetRarity() << std::endl;
	std::cout << "Taken by entity or already purchased: " << node.GetEntityConsumed() << std::endl;
	std::cout << "Currently purchasable: " << node.GetPurchaseAvailability() << std::endl;
	std::cout << "Priority: " << node.GetPriority() << std::endl;
	std::cout << "Parent Node Info: " << std::endl;

	for (Node* p : node.GetParentNodes()) {
		if (p != nullptr) std::cout << "		" <<
			p->GetLocation() << " in ring " <<
			p->GetRing() << " with rarity " <<
			p->GetRarity() << " and availability " <<
			!p->GetEntityConsumed() << " and purchasability " <<
			p->GetPurchaseAvailability() << std::endl;
	}
	std::cout << "Child Node Info: " << std::endl;
	for (Node* c : node.GetChildNodes()) {
		if (c != nullptr) std::cout << "		" <<
			c->GetLocation() << " in ring " <<
			c->GetRing() << " with rarity " <<
			c->GetRarity() << " and availability " <<
			!c->GetEntityConsumed() << " and purchasability " <<
			c->GetPurchaseAvailability() << std::endl;
	}
	std::cout << std::endl << std::endl;
}
