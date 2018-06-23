#include "DataBlock.h"

//	wrap data into data block
void DataBlock::_WrapData(char* &buffer, ModelName name, bool readID) {
	this->model = name;
	if (readID) {
		this->modelID = *(int*)buffer;
		buffer += sizeof(int);
	}
	else this->modelID = 0;
	this->bufferSize = *(int*)buffer;
	buffer += sizeof(int);
}

// static block size
int DataBlock::blockSize = 3 * sizeof(int) + sizeof(std::streamoff);

// serialize datablock object
char* DataBlock::_Serialize() {	
	char* buffer = new char[blockSize];
	char* firstByte = buffer;

	std::memcpy(buffer, &this->model, sizeof(int));
	buffer += sizeof(int);
	std::memcpy(buffer, &this->bufferSize, sizeof(int));
	buffer += sizeof(int);
	std::memcpy(buffer, &this->nextNode, sizeof(std::streamoff));
	buffer += sizeof(std::streamoff);
	std::memcpy(buffer, &this->modelID, sizeof(int));
	
	return firstByte;
}

// deserialize datablock object
void DataBlock::_Deserialize(char* page){
	this->model = *(ModelName*)page;
	page += sizeof(int);
	this->bufferSize = *(int*)page;
	page += sizeof(int);
	this->nextNode = *(std::streamoff*)page;
	page += sizeof(std::streamoff);
	this->modelID = *(int*)page;

	this->blockSize = 3*sizeof(int) + sizeof(std::streamoff);
}