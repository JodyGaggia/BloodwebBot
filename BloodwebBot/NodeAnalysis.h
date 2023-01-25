#pragma once

// Finds nodes in a screenshot of the bloodweb
std::vector<cv::Vec3f> GetNodeLocations(cv::Mat& bloodweb);

// Crops the bloodweb to a specific node
cv::Mat CropBloodwebToNode(cv::Mat& imgBloodweb, Node& node);

// Converts (x,y) coordinates to a node object
Node ConvertLocationToNode(cv::Vec3f nodeLocationInfo);

// Returns a node object containing the node's location on screen and its rarity
void SetNodeRarity(cv::Mat& imgBloodweb, Node& node);

// Marks nodes taken by the entity as unavailable
void UpdateEntityNodes(cv::Mat& bloodweb, std::vector<Node>& vectorOfNodes);

// Checks if the node is one the user has chosen to prioritise
void SetNodePriority(cv::Mat& imgBloodweb, Node& node);

// Finds all nodes which could potentially be dependencies
std::vector<std::reference_wrapper<Node>> FindPotentialDependencyNodes(Node node, std::vector<Node>& vectorOfNodes);

// Determines all dependencies of a node
void DetermineDependency(Node& node, std::vector<std::reference_wrapper<Node>>& potentialDependencies, cv::Mat bloodwebCanny);

