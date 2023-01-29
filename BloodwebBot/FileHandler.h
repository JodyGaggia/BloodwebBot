#pragma once
#include<vector>
#include<iostream>

// Creates resources directory if it does not already exist
void PerformFirstTimeSetup();

/**
* Returns a vector containing paths to all files in a directory
*
* @param[in] path: directory to look for files in
* @return std::vector<string> vector of paths to files in directory
*/
std::vector<std::string> ListFiles(std::string path);

// Writes all variables in Constants.h to Resources/config.txt
void WriteVariablesToConfigFile();
