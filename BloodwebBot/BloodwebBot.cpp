#include <iostream>
#include <opencv2/imgproc.hpp>
#include <string>
#include "Node.h"
#include "Helpers.h"
#include "PurchaseHandler.h"

// Only need one instance of these in memory
static cv::Mat bloodweb;
static std::vector<Node> bloodwebNodes;
static std::vector<std::reference_wrapper<Node>> priorityNodes;

extern bool debug = false;

int main()
{
	PerformFirstTimeSetup();

	while (true) {
		std::system("cls");
		int userResponse;
		std::cout << R"(
     ________  ___       ________  ________  ________  ___       __   _______   ________         
    |\   __  \|\  \     |\   __  \|\   __  \|\   ___ \|\  \     |\  \|\  ___ \ |\   __  \        
    \ \  \|\ /\ \  \    \ \  \|\  \ \  \|\  \ \  \_|\ \ \  \    \ \  \ \   __/|\ \  \|\ /_       
     \ \   __  \ \  \    \ \  \\\  \ \  \\\  \ \  \ \\ \ \  \  __\ \  \ \  \_|/_\ \   __  \      
      \ \  \|\  \ \  \____\ \  \\\  \ \  \\\  \ \  \_\\ \ \  \|\__\_\  \ \  \_|\ \ \  \|\  \     
       \ \_______\ \_______\ \_______\ \_______\ \_______\ \____________\ \_______\ \_______\    
        \|_______|\|_______|\|_______|\|_______|\|_______|\|____________|\|_______|\|_______|    
                                   ________  ________  _________                                 
                                  |\   __  \|\   __  \|\___   ___\                               
                                  \ \  \|\ /\ \  \|\  \|___ \  \_|                               
                                   \ \   __  \ \  \\\  \   \ \  \                                
                                    \ \  \|\  \ \  \\\  \   \ \  \                               
                                     \ \_______\ \_______\   \ \__\                              
                                      \|_______|\|_______|    \|__|                              
                                                                                                 
                                                                                                   
)" << '\n';

		std::cout << "1. Purchase Priority Nodes" << std::endl;
		std::cout << "2. How To Prioritise Certain Items" << std::endl;
		std::cout << "3. Enable Debug Mode" << std::endl;
		std::cin >> userResponse;

		if (userResponse == 1) {
			FillNodesVector(bloodwebNodes, bloodweb);
			PurchasePriorityNodes(bloodwebNodes, priorityNodes, bloodweb);
		}
		else if (userResponse == 2) {
			std::cout << "You can find the required images for items, addons, and offerings respectively from the links below," << std::endl;
			std::cout << "https://deadbydaylight.fandom.com/wiki/Items" << std::endl;
			std::cout << "https://deadbydaylight.fandom.com/wiki/Add-ons" << std::endl;
			std::cout << "https://deadbydaylight.fandom.com/wiki/Offerings" << std::endl;
			std::cout << "Ctrl + click the image and download the 256x256 PNG." << std::endl;
			std::cout << "Drag these into their respective folders in the Resources folder." << std::endl;
			system("pause");
		}
		else {
			debug = true;
		}
	}
}