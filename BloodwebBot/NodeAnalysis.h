#pragma once

/**
* Finds the location of all nodes in the given screenshot
*
* @param[in] bloodweb: image of bloodweb area
* @return Vector of 3-dimensional vectors which each store x,y of center point and detected radius of a node
*/
std::vector<cv::Vec3f> GetNodeLocations(cv::Mat bloodweb);

/**
* Crops a bloodweb screenshot to a given node
*
* @param[in] imgBloodweb: image of the bloodweb
* @param[in] nodeLocationInfo: 3-dimensional vector storing centre point and radius of node
* @return cv::Mat: image of given node (80x80)
*/
cv::Mat CropBloodwebToNode(cv::Mat& imgBloodweb, Node& node);

/**
* Creates a node object using detected location
*
* @param[in] nodeLocationInfo: a 3-dimensional vector storing the x,y coordinates of the center position and the radius of the node
*/
Node ConvertLocationToNode(cv::Vec3f nodeLocationInfo);

/**
* Uses an image of the bloodweb to get average hue of a node to determine rarity
*
* @param[in] imgBloodweb: a screenshot of the bloodweb
* @param[in] node: Node to set the rarity of
* @return void
*/
void SetNodeRarity(cv::Mat& imgBloodweb, Node& node);

/*
* Determines which nodes have been consumed by the entity and updates the 'unattainable' property on these node objects
*
* @param[in] bloodweb: Screenshot of the bloodweb
* @param[in] vectorOfNodes: A vector of all the nodes which have been detected in the screenshot
*/
void UpdateEntityNodes(cv::Mat& bloodweb, std::vector<Node>& vectorOfNodes);

/**
 * Sets the 'isPriority' parameter on the input node object using images present in /Resources/
 *
 * @param[in] imgBloodweb: Screenshot of the bloodweb
 * @param[in] node: Node to set the priority of
 */
void SetNodePriority(cv::Mat& imgBloodweb, Node& node);

/*
* Returns the two nearest nodes in the ring immediately beneath that of the input node. This can be used to detect pathways between nodes.
*
* @param[in] node: Node to find potential parents of
* @param[in] vectorOfNodes: Vector of all detected nodes
* @return std::vector<std::reference_wrapper<Node>>: A vector of references (of nodes in vectorOfNodes) to the two nearest nodes in the ring immediately beneath
* the input
*/
std::vector<std::reference_wrapper<Node>> FindPotentialDependencyNodes(Node node, std::vector<Node>& vectorOfNodes);

/*
* Assigns parent nodes to the input node and, if a node is a parent, assigns this node as its child
*
* 1. Canny the image
* 2. Mask out the space between the two nodes (the input node and its potential parent)
* 3. Look at the mean colour - if it's above some threshold then determine that a connection is present
*
* @param[in] node: Node to find parents of
* @param[in] potentialDependencies: Potential parents of this node. Find these using FindPotentialDependencyNodes
* @param[in] bloodwebCanny: A cannied image of the bloodweb
* @return void
*/
void DetermineDependency(Node& node, std::vector<std::reference_wrapper<Node>>& potentialDependencies, cv::Mat bloodwebCanny);

