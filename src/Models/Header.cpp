#include "Header.h"
#include "../IO/Input.h"
using namespace std;

char* MainHeader::_Serialize() {
	int versionSize = utility::_CharSize(::version);
	int size =  versionSize + sizeof(int); //version char* + size info int
	size += 2 * sizeof(int); //model headers number + next header page num

	//write size at beggining of buffer
	char* buffer = new char[size + sizeof(int)]; 
	std::memcpy(buffer, &size, sizeof(int));
	buffer += sizeof(int);

	//store version size 
	std::memcpy(buffer, &versionSize, sizeof(int));
	buffer += sizeof(int);

	//store version into buffer
	for (int i = 0; i < versionSize; i++)
		*buffer++ = version[i];

	//create place for active model types number
	std::memcpy(buffer, &nodeCount, sizeof(int));
	buffer += sizeof(int);

	//create place to store next header page number
	std::memcpy(buffer, &nextNodePage, sizeof(int));

	buffer -= size;
	return buffer;
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
			this->nodeCount = *(int*)page;
			page += sizeof(int);
			this->nextNodePage = *(int*)page;
			page += sizeof(int);
			this->valid = true;
		}
	}
}