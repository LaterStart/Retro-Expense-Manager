#pragma once
#include <fstream>
#include "../config.h"

const enum class Model {
	none,
	profile,
	account,
	transaction
};

class DataBlock {
protected:
	int blockSize;
};

class Header : public DataBlock{
	friend class Controller;
private:
	int modelCount;
	std::streamoff modelCountPos = 0;
	std::streamoff nextHeaderPos = -1;
	std::streamoff lastHeaderPos = -1;
	std::streamoff lastHeader = -1;

protected:
	std::streamoff nextHeader = -1;	
};

class ModelHeader : public Header {
	friend class Controller;
private:
	Model type = Model::none;	
	int recordsCount = 0;
	std::streamoff firstNode = -1;	

public:
	ModelHeader(Model model) : type(model){}
};

class UserInput;
class Controller {
protected:	
	static Header header;

	Model model;

	Controller() = default;
	const char* filePath = ::database;
	std::ifstream* _Read();
	std::ofstream* _Write();
	std::fstream* _Edit();

	void _WriteHeader(std::ofstream* stream);
	char* _GetHeader(std::ifstream* stream);
	bool _ValidateVersion(std::ifstream* stream = nullptr);

	void _AddNewModelHeader(std::ofstream* stream, ModelHeader& header);

	int _GetModelCount(std::ifstream* stream);
	char* _GetModelHeader(std::ifstream* stream, Model model);
	char* _GetModels(std::ifstream* stream, Model model);

public:
	void _SetFilePath(const char* filePath);
};

inline void Controller::_SetFilePath(const char* filePath) {
	this->filePath = filePath;
}