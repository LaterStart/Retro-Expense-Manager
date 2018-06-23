#include "DataBlock.h"

char* DataBlock::_Serialize() {
	this->blockSize = 2 * sizeof(int) + sizeof(std::streamoff);
	char* buffer = new char[blockSize];
	std::memcpy(buffer, &this->type, sizeof(int));
	buffer += sizeof(int);
	std::memcpy(buffer, &this->bufferSize, sizeof(int));
	buffer += sizeof(int);
	std::memcpy(buffer, &this->nextNode, sizeof(std::streamoff));

	buffer -= (2*sizeof(int));
	return buffer;
}
void DataBlock::_Deserialize(char* page){
	this->type = *(ModelName*)page;
	page += sizeof(int);
	this->bufferSize = *(int*)page;
	page += sizeof(int);
	this->nextNode = *(std::streamoff*)page;
	this->blockSize = 2*sizeof(int) + sizeof(std::streamoff);
}