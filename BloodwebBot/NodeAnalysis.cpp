#include <iostream>
#include <string>
#include <opencv2/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>
#include "Node.h"
#include "NodeAnalysis.h"
#include "FileManagement.h"
#include "Helpers.h"
#include "FileHandler.h"

extern bool debug;

/**
* Copyright (c) 2014, Jan Winkler <winkler@cs.uni-bremen.de>
* All rights reserved. -- https://gist.github.com/fairlight1337/4935ae72bcbcc1ba5c72
* Converts RGB values to HSV values
*
* @param[in] red value in [0,1]
* @param[in] green value in [0,1]
* @param[in] blue value in [0,1]
* @return HSVColor relating to initial RGB values
*/
struct HSVColor {
	float h;
	float s;
	float v;
};
HSVColor RGBtoHSV(float& fR, float& fG, float& fB) {
	float fH = 0, fS = 0, fV = 0;

	float fCMax = std::max(std::max(fR, fG), fB);
	float fCMin = std::min(std::min(fR, fG), fB);
	float fDelta = fCMax - fCMin;

	if (fDelta > 0) {
		if (fCMax == fR) fH = 60 * (fmod(((fG - fB) / fDelta), 6));
		else if (fCMax == fG) fH = 60 * (((fB - fR) / fDelta) + 2);
		else if (fCMax == fB) fH = 60 * (((fR - fG) / fDelta) + 4);

		if (fCMax > 0) fS = fDelta / fCMax;
		else fS = 0;

		fV = fCMax;
	}
	else {
		fH = 0;
		fS = 0;
		fV = fCMax;
	}

	if (fH < 0) fH = 360 + fH;

	return HSVColor{ fH,fS,fV };
}

/**
* Finds the location of all nodes on screen
*
* @param[in] imgBloodwebCanny: cannied image of bloodweb area
* @return Vector of 3-dimensional vectors which each store x,y of center point and detected radius of a node (IN THE GIVEN SCREENSHOT)
*/
std::vector<cv::Vec3f> GetNodeLocations(cv::Mat& imgBloodwebCanny) {
	// Create a vector of vectors storing circle information
	// [0] x_center [1] y_center [2] radius
	std::vector<cv::Vec3f> allNodesLocations;
	cv::HoughCircles(imgBloodwebCanny, allNodesLocations, cv::HOUGH_GRADIENT, 1, imgBloodwebCanny.rows / 24, 100, 30, 32, 45);

	return allNodesLocations;
}

/**
* Crops bloodweb to a given node
*
* @param[in] imgBloodweb: image of the bloodweb
* @param[in] nodeLocationInfo: 3-dimensional vector storing centre point and radius of node IN THE BLOODWEB IMAGE
* @return image of given node (80x80)
*/
cv::Mat CropBloodwebToNode(cv::Mat& imgBloodweb, Node& node) {
	int startX, endX, startY, endY;
	int radius = 40;

	// Set crop region
	startX = node.GetLocation().x - radius;
	startY = node.GetLocation().y - radius;
	endX = node.GetLocation().x + radius;
	endY = node.GetLocation().y + radius;

	// Ensure crop region is within image
	if (startX < 0) startX = 0;
	if (startY < 0) startY = 0;
	if (endX > 810) endX = 810;
	if (endY > 860) endY = 860;

	// Crop to region
	cv::Mat croppedImage = imgBloodweb(cv::Range(startY, endY), cv::Range(startX, endX));

	return croppedImage;
}

/**
* Creates a node object using detected location
*
* @param[in] nodeLocationInfo: a 3-dimensional vector storing the x,y coordinates of the center position and the radius of the node
*/
Node ConvertLocationToNode(cv::Vec3f nodeLocationInfo) {
	return Node(cv::Point(nodeLocationInfo[0], nodeLocationInfo[1]));
}

/**
* Uses an image of the bloodweb to get average hue of a node to determine rarity
*
* @param[in] imgBloodweb: a screenshot of the bloodweb
* @param[in] node: Node to set the rarity of
* @return void
*/
void SetNodeRarity(cv::Mat& imgBloodweb, Node& node) {
	// Hue thresholds for identifying node rarity
	int commonHue = 40,
		uncommonHue = 75, rareHue = 165,
		veryrareHue = 330,
		iridescentHue = 349;

	// Node information
	cv::Point center(node.GetLocation().x, node.GetLocation().y);
	int radius = 35;

	// Node properties
	int rarity = 1;
	bool available = false;

	// Prepare a mask for bloodweb image of only the node (black image coloured white where the node is)
	cv::Mat contourMask = cv::Mat::zeros(imgBloodweb.size(), CV_8UC1);
	cv::circle(contourMask, center, radius, cv::Scalar(255, 255, 255), cv::FILLED, cv::LINE_AA);

	// Grab the mean colour of the node in BGR using the above mask
	cv::Scalar meanCol = mean(imgBloodweb, contourMask);

	// Convert RGB values to [0, 1]
	float r = (float)meanCol[2] / 255;
	float g = (float)meanCol[1] / 255;
	float b = (float)meanCol[0] / 255;

	// Convert from RGB to HSV
	HSVColor hsv = RGBtoHSV(r, g, b);

	// Determine rarity based on hue
	if (hsv.h <= commonHue || hsv.h >= iridescentHue) rarity = 1;
	else if (hsv.h <= uncommonHue) rarity = 2;
	else if (hsv.h <= rareHue) rarity = 3;
	else if (hsv.h <= veryrareHue) rarity = 4;
	else {
		rarity = 5;
		std::cout << "HUE: " << hsv.h << std::endl;
	}

	node.SetRarity(rarity);
}

void UpdateEntityNodes(cv::Mat& bloodweb, std::vector<Node>& vectorOfNodes) {
	cv::Mat grayscaleBloodweb, grayscaleBlurBloodweb, thresholdImage;
	cv::Mat imgEdit = bloodweb.clone();

	cv::cvtColor(bloodweb, grayscaleBloodweb, cv::COLOR_BGR2GRAY);
	cv::GaussianBlur(grayscaleBloodweb, grayscaleBlurBloodweb, cv::Size(0, 0), 3);
	cv::threshold(grayscaleBlurBloodweb, thresholdImage, 10, 255, cv::THRESH_BINARY);

	// Create a vector of vectors storing circle information
	// [0] x_center [1] y_center [2] radius
	std::vector<cv::Vec3f> entityNodes;
	cv::HoughCircles(thresholdImage, entityNodes, cv::HOUGH_GRADIENT, 1, 80, 100, 10, 20, 35);

	if (entityNodes.size() == 0) return;

	// Remove available nodes from unavailable nodes vector
	for (int i = 0; i < entityNodes.size(); i++) {
		// Put location of entity node into a variable
		cv::Vec3f entityNodeInfo = entityNodes[i];
		cv::Point entityNodeLocation(entityNodeInfo[0], entityNodeInfo[1]);

		for (int j = 0; j < vectorOfNodes.size(); j++) {
			// Skip bloodweb node if unavailable
			if (vectorOfNodes[j].GetEntityConsumed() == true) continue;

			cv::Point location = vectorOfNodes[j].GetLocation();

			double centerDistance = cv::norm(entityNodeLocation - location);

			if (centerDistance < 40) {
				vectorOfNodes[j].SetEntityConsumed(true);
				vectorOfNodes[j].SetPurchaseAvailability(false);

				std::cout << "PURCHASE:" << std::endl;
				std::cout << "	Ring: " << vectorOfNodes[j].GetRing() << std::endl;
				std::cout << "	Rarity: " << vectorOfNodes[j].GetRarity() << std::endl;
				std::cout << "	Angle: " << vectorOfNodes[j].GetAngleFromCenter() << std::endl << std::endl;
				break;
			}
		}
	}

	// DEBUG: Draw circles around detected nodes
	if (debug) {
		for (cv::Vec3f node : entityNodes) {
			cv::circle(imgEdit, cv::Point(node[0], node[1]), node[2], cv::Scalar(0, 0, 255), 3);
		}

		cv::imshow("Detected Entity Nodes", imgEdit);
		cv::waitKey(0);
	}
}

/**
 * Returns a vector of matching keypoints between an item in the bloodweb and a reference item image
 *
 * @param values Container whose values are summed.
 * @return sum of `values`, or 0.0 if `values` is empty.
 */
void SetNodePriority(cv::Mat& imgBloodweb, Node& node) {
	// Crop bloodweb to node
	cv::Mat itemInBloodweb = CropBloodwebToNode(imgBloodweb, node);
	
	// Grab node rarity
	int itemRarity = node.GetRarity();

	// Determine where to look for images based on rarity (optimisation)
	std::string path;
	switch (itemRarity) {
	case 1:
		path = "Resources/Common";
		break;
	case 2:
		path = "Resources/Uncommon";
		break;
	case 3:
		path = "Resources/Rare";
		break;
	case 4:
		path = "Resources/Very Rare";
		break;
	case 5:
		path = "Resources/Iridescent";
		break;
	}

	// Convert file paths to a string of vectors
	std::vector<std::string> files = ListFiles(path);

	// If there are no images in the folder, we can just return here
	if (files.size() == 0) {
		node.SetPriority(false);
		return;
	}

	for (std::string file : files) {
		// Prepare Mats for processing
		cv::Mat itemInBloodweb_Zoomed, itemInBloodweb_ZoomedSharpened, itemInBloodweb_Gray, itemInBloodwebDescriptors,
			itemToFind, itemToFind_Descriptors;

		std::vector<cv::KeyPoint> itemInBloodweb_Keypoints, itemToFind_Keypoints;

		// Prepare item in bloodweb for image analysis
		// Zoom in, sharpen, grayscale
		cv::pyrUp(itemInBloodweb, itemInBloodweb_Zoomed, cv::Size(itemInBloodweb.cols * 2, itemInBloodweb.rows * 2)); // Increases node to 180x180
		cv::GaussianBlur(itemInBloodweb_Zoomed, itemInBloodweb_ZoomedSharpened, cv::Size(0, 0), 3);
		cv::addWeighted(itemInBloodweb_Zoomed, 2, itemInBloodweb_ZoomedSharpened, -0.5, 0, itemInBloodweb_ZoomedSharpened);
		cv::cvtColor(itemInBloodweb_ZoomedSharpened, itemInBloodweb_Gray, cv::COLOR_BGR2GRAY);

		// Grab reference image and resize to size of item in bloodweb
		itemToFind = cv::imread(file, cv::IMREAD_GRAYSCALE);
		cv::pyrDown(itemToFind, itemToFind, cv::Size(itemToFind.cols / 2, itemToFind.rows / 2)); // Our reference images are ~300x300 so reduce to 150x150

		// Detect keypoints and compute detectors
		cv::Ptr<cv::FeatureDetector> detector = cv::ORB::create();
		detector->detect(itemInBloodweb_Gray, itemInBloodweb_Keypoints);
		detector->detect(itemToFind, itemToFind_Keypoints);

		cv::Ptr<cv::DescriptorExtractor> extractor = cv::ORB::create();
		extractor->compute(itemInBloodweb_Gray, itemInBloodweb_Keypoints, itemInBloodwebDescriptors);
		extractor->compute(itemToFind, itemToFind_Keypoints, itemToFind_Descriptors);

		// Match keypoints
		cv::Ptr<cv::BFMatcher> matcher = cv::BFMatcher::create(cv::NORM_HAMMING, false);
		std::vector<cv::DMatch> matches;
		matcher->match(itemToFind_Descriptors, itemInBloodwebDescriptors, matches);

		// Filter matches to minimum distance, form "good" matches
		//double min_dist = 100;
		//for (int i = 0; i < itemToFind_Descriptors.rows; i++) {
		//	if (matches[i].distance < min_dist) min_dist = matches[i].distance;
		//}

		// Create a vector storing all good matches
		std::vector<cv::DMatch> good_matches;
		for (int i = 0; i < itemToFind_Descriptors.rows; i++)
		{
			if (matches[i].distance <= 20)
				good_matches.push_back(matches[i]);
		}

		// Set all the items with "good" matches to be priority items
		if (good_matches.size() > 0) {
			if (debug) {
				cv::Mat img_matches;
				cv::drawMatches(itemToFind, itemToFind_Keypoints, itemInBloodweb_Gray, itemInBloodweb_Keypoints,
					good_matches, img_matches, cv::Scalar(255, 0, 255));
				cv::imshow("Matching Keypoints", img_matches);
				cv::waitKey(0);
			}
			node.SetPriority(true);
			return;
		}
	}
	node.SetPriority(false);
	return;
}

int GetDistance(Node& node1, Node& node2) {
	return cv::norm(node1.GetLocation() - node2.GetLocation());
}

std::vector<std::reference_wrapper<Node>> FindPotentialDependencyNodes(Node node, std::vector<Node>& vectorOfNodes) {	
	// Initialise vector of potential nodes
	std::vector<std::reference_wrapper<Node>> closestNodes;

	// Location of input node
	cv::Point inputNodeLocation(node.GetLocation().x, node.GetLocation().y);

	// Only look at the inner ring (1 down) for dependencies
	int ring = node.GetRing() - 1;
	if (ring == 0) return closestNodes;

	// Add all nodes in inner ring to vector
	for (Node& aNode : vectorOfNodes) {
		if(aNode.GetRing() == ring) closestNodes.push_back(aNode);
	}

	// Sort vector based on distance to node
	std::sort(closestNodes.begin(), closestNodes.end(), [&node](Node& node1, Node& node2) { return GetDistance(node, node1) < GetDistance(node, node2); });

	// Choose the nearest two nodes as potential dependencies
	closestNodes.erase(closestNodes.begin() + 2, closestNodes.end());

	return closestNodes;
}

void DetermineDependency(Node& node, std::vector<std::reference_wrapper<Node>>& potentialDependencies, cv::Mat bloodwebCanny) {
	// Center point of current node
	cv::Point nodeCenter(node.GetLocation().x, node.GetLocation().y);
	cv::Mat cannyClone = bloodwebCanny.clone();

	for (int i = 0; i < potentialDependencies.size(); i++) {
		// Center point of potential dependency
		cv::Point dependencyCenter(potentialDependencies[i].get().GetLocation().x, potentialDependencies[i].get().GetLocation().y);

		// Mask out the nodes themselves since they can interfere with the mean colour grabbed later on
		cv::circle(cannyClone, dependencyCenter, 40, cv::Scalar(0, 0, 0), cv::FILLED);
		cv::circle(cannyClone, nodeCenter, 40, cv::Scalar(0, 0, 0), cv::FILLED);

		// Create a mask where the line for a connection would be between the nodes
		cv::Mat contourMask = cv::Mat::zeros(cannyClone.size(), CV_8UC1);
		cv::line(contourMask, nodeCenter, dependencyCenter, cv::Scalar(255, 255, 255), 3);

		// Grab the mean colour of the node connection in BGR
		cv::Scalar meanCol = mean(cannyClone, contourMask);

		if (meanCol[0] > 18) {
			// Add current node as a child node to the dependency
			potentialDependencies[i].get().AddChildNode(&node);
			// Add dependency node as parent of current node
			node.AddParentNode(&potentialDependencies[i].get());
		}
	}
	//std::cout << "\n" << std::endl;
}
