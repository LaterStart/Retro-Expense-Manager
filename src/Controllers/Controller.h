#pragma once
#include <fstream>
class Controller {
protected:	
	Controller();
	const char* filePath;
	bool status;
	std::ifstream* _OpenFile();

public:
	void _SetFilePath(const char* filePath);
	bool _GetStatus() const;
};

inline bool Controller::_GetStatus() const {
	return status;
}

