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

const enum class Range {
	none,
	all
};

class UserInput;
struct Data {
	Field field;
	UserInput* input;

public:
	Data(Field field, UserInput* input) : field(field), input(input) {}
	~Data() = default;
};

class Query {
	Range range = Range::none;

public:
	void _SetRange(Range range);
	Query() = default;
	Query(Range range) : range(range){}
	~Query() = default;
};

inline void Query::_SetRange(Range range) {
	this->range = range;
}

class DataBlock; class Header; class MainHeader; class ModelHeader;
class Controller {
	friend void Initialize::_Controller();
private:
	const char* filePath = ::database;
	const int clusterSize = 4096;
	static MainHeader header;	
	void _LoadHeader();	

	DataBlock _GetBlock(char* page, int offset, ModelName name);
	void _DeleteBuffers(char* &buffer, char* &dblock, bool containsID = true);
	void _UpdateHeader(std::fstream* stream, Header& header);
	void _UpdateLastNode(std::fstream* stream, Header& header, ModelName name, std::streamoff pageNum);	

	Controller(bool& initialize);

protected:		
	ModelName model = ModelName::none;
	
	std::fstream* _OpenStream();
	bool _CreateDatabase();
	void _WriteNewModelHeader(std::fstream* stream, ModelHeader& header);
	void _WriteModel(std::fstream* stream, ModelHeader& header, char* buffer);
	void _LoadHeader(ModelHeader& header);
	char** _GetModels(std::fstream* stream, ModelHeader& header, Query query);
	char* _GetModel(std::fstream* stream, ModelHeader& header, int ID);
	void _UpdateModel(std::fstream* stream, ModelHeader& header, int ID, char* buffer);
	
	Controller() = default;
	~Controller() = default;

public:
	void _SetFilePath(const char* filePath);
};

inline void Controller::_SetFilePath(const char* filePath) {
	this->filePath = filePath;
}