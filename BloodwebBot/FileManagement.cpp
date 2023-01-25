#include <filesystem>
#include <string>
#include <vector>

std::vector<std::string> ListFiles(std::string path) {
	std::vector<std::string> files;

	for (const auto& entry : std::filesystem::directory_iterator(path)) {
		files.push_back(entry.path().u8string());
	}

	return files;
}