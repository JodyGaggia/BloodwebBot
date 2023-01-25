#include <iostream>
#include <opencv2/highgui.hpp>
#include<memory>
#include "Node.h"
#include "Constants.h"

/* NODE PROPERTIES
* 
*	MAIN:
* 	cv::Point location;
*	bool accessible;
*   bool purchasable;
*	int priority;
*	std::vector<Node*> childNodes;
*	std::vector<Node*> parentNodes;
* 
*	DEBUG:
*	int distanceFromCenter;
*	float angleFromCenter;
*	int ring;
*	int rarity;
* 
*/ 

void SetDistanceAndAngle(int& distanceFromCenter, float& angleFromCenter, cv::Point& location);
void SetRingNumber(int& ring, int distanceFromCenter);

Node::Node(cv::Point loc) {
	// Set the location property
	location = loc;

	// Set the distanceFromCenter and angleFromCenter property
	SetDistanceAndAngle(distanceFromCenter, angleFromCenter, location);

	// Set the ring property
	SetRingNumber(ring, distanceFromCenter);
}

void SetDistanceAndAngle(int& distanceFromCenter, float& angleFromCenter, cv::Point& location) {
	// Center of bloodweb
	cv::Point centerPoint(BLOODWEB_CENTER_X, BLOODWEB_CENTER_Y);

	// Distance to center point
	int radius = cv::norm(location - centerPoint);

	// Angle between node and center point
	float angle = std::atan2(location.y - centerPoint.y, location.x - centerPoint.x);

	distanceFromCenter = radius;
	angleFromCenter = (angle * (180 / 3.14159));
}

void SetRingNumber(int& ring, int distanceFromCenter) {
	if (distanceFromCenter < 200) ring = 1;
	else if (distanceFromCenter > 200 && distanceFromCenter < 300) ring = 2;
	else ring = 3;
}

cv::Point Node::GetLocation() {
	return location;
}
int Node::GetDistanceFromCenter() {
	return distanceFromCenter;
}
float Node::GetAngleFromCenter() {
	return angleFromCenter;
}
int Node::GetRing() {
	return ring;
}
void Node::SetEntityConsumed(bool canAccess) {
	unattainable = canAccess;
}
bool Node::GetEntityConsumed() {
	return unattainable;
}
void Node::SetPurchaseAvailability(bool canPurchase) {
	purchasable = canPurchase;
}
bool Node::GetPurchaseAvailability() {
	return purchasable;
}
void Node::SetRarity(int rarityToSet) {
	rarity = rarityToSet;
}
int Node::GetRarity() {
	return rarity;
}
bool Node::GetPriority() {
	return priority;
}
void Node::SetPriority(bool isPriority) {
	priority = isPriority;
}
std::vector<Node*> Node::GetChildNodes() {
	return childNodes;
}
void Node::AddChildNode(Node* ptrChildNode) {
	childNodes.push_back(ptrChildNode);
}
std::vector<Node*> Node::GetParentNodes() {
	return parentNodes;
}
void Node::AddParentNode(Node* ptrParentNode) {
	parentNodes.push_back(ptrParentNode);
}