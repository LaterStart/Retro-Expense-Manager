#include <fstream>
#include "_Controller.h"
#include "../Models/Header.h"
#include "../Models/DataBlock.h"
using namespace std;

//	static header class instance - stores frequently accessed database data
MainHeader Controller::header;

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
	this->bufferSize = *(int*)buffer;
	buffer += sizeof(int);
}

void Controller::_DeleteBuffers(char* &buffer, char* &dblock) {
	buffer -= sizeof(int);
	delete[]buffer;
	delete[]dblock;
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
		block._WrapData(buffer, ModelName::mainHeader);

		//write data block
		char* dblock = block._Serialize();
		streamoff pos = stream.tellp();
		stream.write(dblock, block._BlockSize());
		stream.write(buffer, block._BufferSize());
		header._SetPosition(pos);
		
		stream.close();
		_DeleteBuffers(buffer, dblock);
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

//	finds requested block name on the given page buffer
DataBlock Controller::_GetBlock(char* page, ModelName name) {
	int pos = 0;
	do {
		DataBlock block;
		block._Deserialize(page);
		if (block._Type() == name) {
			block._SetOffset(pos);
			block.empty = false;
			return block;
		}
		else {
			pos += (block._BlockSize() + block._BufferSize());
			page += (block._BlockSize() + block._BufferSize());
		}
	} while (pos < clusterSize);
	DataBlock empty;
	return empty;
}

//	load main header at application startup
bool Controller::_LoadHeader() {
	fstream* stream = new fstream;
	stream->open(filePath);
	if (stream->is_open()) {
		do {
			streamoff pos = stream->tellg();
			char* page = _ReadPage(stream);
			DataBlock block = _GetBlock(page, ModelName::mainHeader);			
			if (!block.empty) {
				page += block._Offset();
				header._Deserialize(page);
				header._SetPosition(pos + block._Offset() - block._BlockSize());
				page -= block._Offset();
				delete[]page;
				break;
			}			
			delete[]page;
		} while (!stream->eof());
	}
	stream->close();
	delete stream;
	if (header._Status())
		return true;
	else return false;
}

//	updates existing header database record with new value
void Controller::_UpdateHeader(fstream* stream, Header& header) {
	ModelName type = header._Type();
	char* buffer = nullptr;
	if (header._Type() == ModelName::mainHeader) {
		MainHeader* maHeader = dynamic_cast<MainHeader*>(&header);
		buffer = maHeader->_Serialize();
	}
	else if (header._Type() == ModelName::modelHeader) {
		ModelHeader* moHeader = dynamic_cast<ModelHeader*>(&header);
		buffer = moHeader->_Serialize();
	}
	
	if (buffer != nullptr) {
		DataBlock block;
		block._WrapData(buffer, ModelName::mainHeader);
		char* dblock = block._Serialize();

		stream->seekp(header._Position(), ios::beg);
		stream->write(dblock, block._BlockSize());
		stream->write(buffer, block._BufferSize());

		_DeleteBuffers(buffer, dblock);
	}
}

//	writes new model header into database
void Controller::_WriteNewModelHeader(fstream* stream, ModelHeader& header) {
	stream->seekp(0, ios::end);
	char* buffer = header._Serialize();

	DataBlock block;
	block._WrapData(buffer, ModelName::modelHeader);
	char* dblock = block._Serialize();

	streamoff pos = stream->tellp();
	stream->write(dblock, block._BlockSize());
	stream->write(buffer, block._BufferSize());
	header._SetPosition(pos);

	_DeleteBuffers(buffer, dblock);

	this->header._IncreaseNodeCount();
	int pageNum = pos / this->clusterSize;
	if (this->header._NextNodePage() < 0) 		
		this->header._EditNextNodePage(pageNum);
	this->header._EditLastNodePage(pageNum);

	_UpdateHeader(stream, this->header);
}

bool Controller::_LoadHeader(ModelHeader& header) {
	fstream* stream = _OpenStream();



	return true;
}

//	write new model data into database
void Controller::_WriteModel(std::fstream* stream, ModelHeader& header, char* buffer) {
	stream->seekp(0, ios::end);

	DataBlock block;
	block._WrapData(buffer, header._Name());
	char* dblock = block._Serialize();

	streamoff pos = stream->tellp();
	stream->write(dblock, block._BlockSize());
	stream->write(buffer, block._BufferSize());

	_DeleteBuffers(buffer, dblock);

	header._IncreaseNodeCount();
	int pageNum = pos / this->clusterSize;
	if (header._NextNodePage() < 0)
		header._EditNextNodePage(pageNum);
	header._EditLastNodePage(pageNum);

	_UpdateHeader(stream, header);
}

