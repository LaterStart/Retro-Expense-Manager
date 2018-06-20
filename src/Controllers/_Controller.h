#pragma once
#include <fstream>
#include "../config.h"

const enum class ModelName {
	none,
	mainHeader,
	modelHeader,
	profile,
	account,
	transaction
};

const enum class Field {
	none,
	username,
	pwStatus,
	password,
	defCCYid
};

class UserInput;
struct Data {
	Field field;
	UserInput* input;

public:
	Data(Field field, UserInput* input) : field(field), input(input) {}
};

class DataBlock; class Header; class MainHeader; class ModelHeader;
class Controller {
	friend void Initialize::_Controller();
private:
	const char* filePath = ::database;
	const int clusterSize = 4096;
	static MainHeader header;
	void _LoadHeader();	

	char* _ReadPage(std::fstream* stream);
	DataBlock _GetBlock(char* page, ModelName name);
	void _DeleteBuffers(char* &buffer, char* &dblock);
	void _UpdateHeader(std::fstream* stream, Header& header);		

	Controller(bool& initialize);

protected:		
	ModelName model;
	
	std::fstream* _OpenStream();
	bool _CreateDatabase();
	void _WriteNewModelHeader(std::fstream* stream, ModelHeader& header);
	void _WriteModel(std::fstream* stream, ModelHeader& header, char* buffer);
	ModelHeader _LoadHeader(ModelName name);
	
	Controller() = default;
	~Controller() = default;

public:
	void _SetFilePath(const char* filePath);
};

inline void Controller::_SetFilePath(const char* filePath) {
	this->filePath = filePath;
}