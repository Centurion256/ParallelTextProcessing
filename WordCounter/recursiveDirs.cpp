#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>

int readFiles(std::string path)
{
	//std::string myPath = "../test";
	using recursive_directory_iterator = std::filesystem::recursive_directory_iterator;
	for (const auto& dirEntry : recursive_directory_iterator(path))
		if (dirEntry.path().extension() == ".zip") {
			std::cout << dirEntry << std::endl;
			std::ifstream raw_file(dirEntry, std::ios::binary);
			auto buffer = static_cast<std::ostringstream&>(
				std::ostringstream{} << raw_file.rdbuf()).str();
		}
	return 0;
}