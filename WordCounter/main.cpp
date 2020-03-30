#include <iostream>
#include"recursiveDirs.hpp";

int main(int, char**) {
	std::string path = "../test";
	readFiles(path);	
	std::cout << "Hello, world!\n";
	return 0;
}
