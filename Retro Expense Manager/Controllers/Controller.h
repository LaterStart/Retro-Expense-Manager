#pragma once
#include <fstream>
class Controller {
private: 
	const char* filePath;

protected:	
	std::ifstream* _OpenFile();

public:
	void _SetFilePath(const char* filePath);
};

