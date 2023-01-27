#pragma once
#include<map>

// Area of screen which is screenshotted to capture the bloodweb
extern int IMAGE_ORIGIN_X;
extern int IMAGE_ORIGIN_Y;
extern int IMAGE_WIDTH;
extern int IMAGE_HEIGHT;

// Center of the bloodweb in the above screenshot
extern int BLOODWEB_CENTER_X;
extern int BLOODWEB_CENTER_Y;

// Number of bloodpoints to spend
extern int bloodpoints;

// Convert rarity to bloodpoints
const std::map<int, int> rarityToCost = {
	{1, 2000},
	{2, 2500},
	{3, 3250},
	{4, 4000},
	{5, 5000}
};