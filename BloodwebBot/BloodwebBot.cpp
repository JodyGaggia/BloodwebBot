#include <iostream>
#include <opencv2/imgproc.hpp>
#include <string>
#include "Node.h"
#include "Helpers.h"
#include "PurchaseHandler.h"
#include "Calibration.h"
#include "Constants.h"
#include "FileHandler.h"

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

		std::cout << "1. Start Bot" << std::endl;
		std::cout << "2. How To Prioritise Items" << std::endl;
		std::cout << "3. Toggle Debug" << std::endl;
		std::cout << "4. Calibrate" << std::endl;
		std::cin >> userResponse;

		if (userResponse == 1){

			while (1) {
				std::string bloodpointsToSpend;
				bool isNumber;

				std::cout << "Number of bloodpoints to spend: ";
				std::cin >> bloodpointsToSpend;

				for (int i = 0; i < bloodpointsToSpend.length(); i++)
				{
					if (isdigit(bloodpointsToSpend[i]) == false) {
						isNumber = false;
						break;
					}
				}

				if (isNumber) {
					bloodpoints = std::stoi(bloodpointsToSpend);
					break;
				}
			}

			std::system("cls");
			std::cout << "!! IMPORTANT !!" << std::endl;
			std::cout << "1) Hold P at any time to stop the bot. The bot will automatically move your mouse." << std::endl;
			std::cout << "2) Before continuing from this page, make sure the bloodweb is fully visible." << std::endl;
			std::cout << "2) The bot will only work on a bloodweb where no nodes have been purchased." << std::endl;
			std::system("pause");

			FillNodesVector(bloodwebNodes, bloodweb);
			PurchasePriorityNodes(bloodwebNodes, priorityNodes, bloodweb);
		}
		else if (userResponse == 2) {
			std::cout << "You can find the required images for items, addons, and offerings respectively from the links below," << std::endl;
			std::cout << "https://deadbydaylight.fandom.com/wiki/Items" << std::endl;
			std::cout << "https://deadbydaylight.fandom.com/wiki/Add-ons" << std::endl;
			std::cout << "https://deadbydaylight.fandom.com/wiki/Offerings" << std::endl;
			std::cout << "Ctrl + click the image and download as WEBP, PNG or JPEG." << std::endl;
			std::cout << "Drag these into their respective folders in the Resources folder." << std::endl;
			system("pause");
		}
		else if (userResponse == 3){
			debug = !debug;
			std::cout << "Debug: " << debug << std::endl;
			system("pause");
		}
		else {
			Calibrate();
		}
	}
}