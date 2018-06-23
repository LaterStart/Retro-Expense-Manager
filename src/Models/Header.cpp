#include "Header.h"

char* MainHeader::_Serialize() {
	int versionSize = utility::_CharSize(::version);
	//version char* + size info int
	int size =  versionSize + sizeof(int); 
	//model headers count, id count and next & last header page num
	size += 2*sizeof(unsigned int) + 2*sizeof(std::streamoff); 

	//write size and ID at beggining of buffer
	char* buffer = new char[size + sizeof(int)]; 
	char* firstByte = buffer;

	std::memcpy(buffer, &size, sizeof(int));
	buffer += sizeof(int);

	//store version size 
	std::memcpy(buffer, &versionSize, sizeof(int));
	buffer += sizeof(int);

	//store version into buffer
	for (int i = 0; i < versionSize; i++)
		*buffer++ = version[i];

	//create place for active model types number
	std::memcpy(buffer, &nodeCount, sizeof(unsigned int));
	buffer += sizeof(unsigned int);

	//create place for id counter
	std::memcpy(buffer, &idCount, sizeof(unsigned int));
	buffer += sizeof(unsigned int);

	//create place to store next header page  and last header page number
	std::memcpy(buffer, &firstNode, sizeof(std::streamoff));
	buffer += sizeof(std::streamoff);
	std::memcpy(buffer, &lastNode, sizeof(std::streamoff));

	return firstByte;
}

//	deserialize main header object data from database file
void MainHeader::_Deserialize(char* page) {
	int versionSize = utility::_CharSize(::version);	

	int versionReadSize = *(int*)page;
	if (versionReadSize == versionSize) {
		page += sizeof(int);
		char* version = new char[versionReadSize];
		for (int i = 0; i < versionReadSize; i++)
			version[i] = *page++;
		if (utility::_CompareChar(version, ::version)) {
			this->nodeCount = *(unsigned int*)page;
			page += sizeof(unsigned int);
			this->idCount = *(unsigned int*)page;
			page += sizeof(unsigned int);
			this->firstNode = *(std::streamoff*)page;
			page += sizeof(std::streamoff);
			this->lastNode = *(std::streamoff*)page;
		}
	}
}

//	serialize model header object
char* ModelHeader::_Serialize() {
	int size = sizeof(int) + 2* sizeof(std::streamoff) + 2*sizeof(unsigned int);
	char* buffer = new char[size+sizeof(int)];
	char* firstByte = buffer;

	std::memcpy(buffer, &size, sizeof(int));
	buffer += sizeof(int);
	std::memcpy(buffer, &this->model, sizeof(int));
	buffer += sizeof(int);
	std::memcpy(buffer, &this->nodeCount, sizeof(unsigned int));
	buffer += sizeof(unsigned int);
	std::memcpy(buffer, &this->idCount, sizeof(unsigned int));
	buffer += sizeof(unsigned int);
	std::memcpy(buffer, &this->firstNode, sizeof(std::streamoff));
	buffer += sizeof(std::streamoff);
	std::memcpy(buffer, &this->lastNode, sizeof(std::streamoff));

	return firstByte;
}

//	deserialize model header object
void ModelHeader::_Deserialize(char* page) {
	this->model = *(ModelName*)page;
	page += sizeof(int);
	this->nodeCount = *(unsigned int*)page;
	page += sizeof(unsigned int);
	this->idCount = *(unsigned int*)page;
	page += sizeof(unsigned int);
	this->firstNode = *(std::streamoff*)page;
	page += sizeof(std::streamoff);
	this->lastNode = *(std::streamoff*)page;
}