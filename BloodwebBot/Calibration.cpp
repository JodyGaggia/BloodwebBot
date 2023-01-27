#include <Windows.h>
#include <iostream>
#include "Helpers.h"
#include "Constants.h"
#include "FileHandler.h"

void Calibrate() {
	// Grab desktop dimensions
	RECT desktop;
	HWND hDesktop = GetDesktopWindow();
	GetWindowRect(hDesktop, &desktop);

	// Screenshot screen, have user select bloodweb region
	cv::Mat bloodweb = ScreenshotBloodweb(0, 0, desktop.right, desktop.bottom);
	cv::Rect areaOnScreen = cv::selectROI("Select Bloodweb", bloodweb, false);
	cv::destroyAllWindows();

	// Have user select middle of bloodweb
	cv::Mat selectedRegion = bloodweb(cv::Range(areaOnScreen.y, areaOnScreen.y + areaOnScreen.height), cv::Range(areaOnScreen.x, areaOnScreen.x + areaOnScreen.width));
	cv::Rect bloodwebCenter = cv::selectROI("Select Bloodweb Center", selectedRegion, true);
	cv::destroyAllWindows();

	// Update screenshot region constants
	IMAGE_ORIGIN_X = areaOnScreen.x;
	IMAGE_ORIGIN_Y = areaOnScreen.y;
	IMAGE_WIDTH = areaOnScreen.width;
	IMAGE_HEIGHT = areaOnScreen.height;

	BLOODWEB_CENTER_X = bloodwebCenter.x + bloodwebCenter.width / 2;
	BLOODWEB_CENTER_Y = bloodwebCenter.y + bloodwebCenter.height / 2;

	WriteVariablesToConfigFile();

	cv::waitKey(0);
}