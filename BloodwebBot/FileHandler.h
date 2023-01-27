#pragma once
#include<vector>
#include<iostream>

// Creates resources directory if it does not already exist
void PerformFirstTimeSetup();
// Returns paths to all files in a directory
std::vector<std::string> ListFiles(std::string path);
// Writes all variables in Constants.h to Resources/config.txt
void WriteVariablesToConfigFile();
