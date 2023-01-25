#pragma once
#include <opencv2/highgui.hpp>

class Node {
private:
	// Location of the node in the screenshot
	cv::Point location; 
	// Euclidean distance to the center of the bloodweb
	int distanceFromCenter;
	// Angle in degrees from the center of the bloodweb
	float angleFromCenter;
	// The ring in the bloodweb to which the node belongs
	int ring;
	// If the node has been taken by the entity or purchased by the user
	bool unattainable = false;
	// If the node can be purchased right now
	bool purchasable = false; 
	// The rarity of the node
	int rarity;
	// Whether or not the system should prioritise buying this node
	bool priority;
	std::vector<Node*> childNodes;
	std::vector<Node*> parentNodes;

public:
	Node(cv::Point loc);

	// cv::Point: Returns location of the node in the bloodweb screenshot
	cv::Point GetLocation();
	// int: Returns the distance of the node from the center of the bloodweb (euclidean)
	int GetDistanceFromCenter();
	// float: Returns angle of the node from the center of the bloodweb (degrees)
	float GetAngleFromCenter();
	// int: Returns the ring in the bloodweb which the node lies upon
	int GetRing();
	// Set if the node has been taken by the entity or purchased by the user
	void SetEntityConsumed(bool canAccess);
	// bool: Returns if the node has been taken by the entity or purchased by the user
	bool GetEntityConsumed();
	// Set if the node is currently purchasable
	void SetPurchaseAvailability(bool canPurchase);
	// bool: Returns if the node is currently purchasable
	bool GetPurchaseAvailability();
	// Set the rarity of the node
	void SetRarity(int rarityToSet);
	// int: Returns the rarity of the node
	int GetRarity();
	// bool: Returns whether the node is a priority node or not
	bool GetPriority();
	// Set whether or not a node should be prioritised
	void SetPriority(bool isPriority);
	// std::vector<Node*>: Returns all children of this node
	std::vector<Node*> GetChildNodes();
	// Add a node to the list of children for this node
	void AddChildNode(Node* ptrChildNode);
	// std::vector<Node*>: Returns all parents of this node
	std::vector<Node*> GetParentNodes();
	// Add a node to the list of parents for this node
	void AddParentNode(Node* ptrParentNode);
};