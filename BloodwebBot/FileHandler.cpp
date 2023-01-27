#include <iostream>
#include <vector>
#include <fstream>
#include <filesystem>
#include "Constants.h"

void WriteVariablesToConfigFile();
void ReadVariablesFromConfigFileConfigFile();

/**
* Returns a vector containing paths to all files in a directory
*
* @param[in] path: directory to look for files in
* @return std::vector<string> vector of paths to files in directory
*/
std::vector<std::string> ListFiles(std::string path) {
	std::vector<std::string> files;

	for (const auto& entry : std::filesystem::directory_iterator(path)) {
		files.push_back(entry.path().u8string());
	}

	return files;
}

/**
* Creates directories for priority items if they have not already been made
*/
void PerformFirstTimeSetup() {
	if (!std::filesystem::exists("Resources")) {
		std::filesystem::create_directories("Resources");
		std::filesystem::create_directories("Resources/Common");
		std::filesystem::create_directories("Resources/Uncommon");
		std::filesystem::create_directories("Resources/Rare");
		std::filesystem::create_directories("Resources/Very Rare");
		std::filesystem::create_directories("Resources/Iridescent");

		WriteVariablesToConfigFile();

		std::cout << "Looks like it's your first time launching this program!\n\nThe program has created a folder called resources - deletion of this folder (or deletion of the folders inside of it) \nwill result in a crash.\n\nPress 2 on the next screen for more information!\n" << std::endl;;
		system("pause");
	}
	else {
		ReadVariablesFromConfigFileConfigFile();
	}
}

/**
* Writes all variables in Constants.h to Resources/config.txt
*/
void WriteVariablesToConfigFile() {
	std::ofstream out("Resources/config.txt");

	out << IMAGE_ORIGIN_X << "\n" << IMAGE_ORIGIN_Y << "\n" << IMAGE_WIDTH << "\n" << IMAGE_HEIGHT << "\n" << BLOODWEB_CENTER_X << "\n" << BLOODWEB_CENTER_Y << "\n";
	out.close();
}

/**
* Reads all lines from Resources/config.txt to the variables in Constants.h
*/
void ReadVariablesFromConfigFileConfigFile() {
	std::ifstream in("Resources/config.txt");

	in >> IMAGE_ORIGIN_X >> IMAGE_ORIGIN_Y >> IMAGE_WIDTH >> IMAGE_HEIGHT >> BLOODWEB_CENTER_X >> BLOODWEB_CENTER_Y;
	in.close();
}