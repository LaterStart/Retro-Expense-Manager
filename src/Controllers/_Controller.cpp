#include <fstream>
#include "_Controller.h"
#include "../Models/Header.h"
using namespace std;

//	static header class instance - stores frequently accessed database data
Header Controller::header;

//	create fstream instance for binary write and read mode
fstream* Controller::_OpenStream() {
	fstream* stream = new fstream;
	stream->open(filePath, ios::binary | ios::out | ios::in);
	if (stream->is_open())
		return stream;
	else {
		delete stream;
		return nullptr;
	}
}

//	wrap data into data block
void DataBlock::_WrapData(char* &buffer, ModelName name) {
	this->type = name;
	this->blockSize = *(int*)buffer;
	buffer += sizeof(int);
}

//	create database binary file
bool Controller::_CreateDatabase() {	
	ofstream stream;
	stream.open(filePath);
	if (stream.is_open()) {
		//serialize main header data		
		char* buffer = header._Serialize();

		//wrap data into data block
		DataBlock block;
		block._WrapData(buffer, ModelName::header);

		//write data block
		stream.write(reinterpret_cast<char*>(&block), sizeof(DataBlock));
		stream.write(buffer, block._Size());
		
		stream.close();
		delete[]buffer;
		return true;
	}
	return false;
}

//	read one cluster size binary data from file
char* Controller::_ReadPage(fstream* stream) {
	char* buffer = new char[this->clusterSize];
	stream->read(buffer, clusterSize);
	return buffer;
}

bool Controller::_LoadHeader() {
	fstream* stream = new fstream;
	stream->open(filePath);
	if (stream->is_open()) {
		do {
			char* page = _ReadPage(stream);
			DataBlock block = *(DataBlock*)page;
			if (block._Type() == ModelName::header) {
				page += sizeof(DataBlock);
				header._Deserialize(page);
				break;
			}
		} while (true);
		//deserialize main header data
		//header._Deserialize(stream);
		return true;
	}
	return false;
}