#pragma once
#include <fstream>
#include "../config.h"

const enum class ModelName {
	none,
	header,
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

class DataBlock {
private:
	ModelName type = ModelName::none;
	int blockSize = 0;

public:
	void _WrapData(char* &buffer, ModelName name);
	int _Size() const;
	ModelName _Type() const;
};

inline int DataBlock::_Size() const {
	return blockSize;
}

inline ModelName DataBlock::_Type() const {
	return type;
}

class Header;
class Controller {
private:
	const char* filePath = ::database;
	const int clusterSize = 4096;
	static Header header;

	char* _ReadPage(std::fstream* stream);
protected:		
	ModelName model;
	
	std::fstream* _OpenStream();
	bool _CreateDatabase();

public:
	void _SetFilePath(const char* filePath);
	bool _LoadHeader();

	Controller() = default; //test main - put private
	~Controller() = default; //test main
};

inline void Controller::_SetFilePath(const char* filePath) {
	this->filePath = filePath;
}