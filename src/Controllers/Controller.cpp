#include <fstream>
#include "Controller.h"
using namespace std;

//	static header class instance
Header Controller::header;

//	create ofstream instance for binary write mode
ofstream* Controller::_Write() {
	ofstream* stream = new ofstream;
	stream->open(filePath, ios::binary | ios::app);
	if (stream->is_open())
		return stream;
	else {
		delete stream;
		return nullptr;
	}
}

//	create fstream instance for binary write and read mode
fstream* Controller::_Edit() {
	fstream* stream = new fstream;
	stream->open(filePath, ios::binary | ios::in | ios::out);
	if (stream->is_open())
		return stream;
	else {
		delete stream;
		return nullptr;
	}
}

//	create ifstream instance for binary read mode
ifstream* Controller::_Read() {
	ifstream* stream = new ifstream;
	stream->open(filePath, ios::binary);
	if (stream->is_open())
		return stream;
	else {
		delete stream;
		return nullptr;
	}
}

//	write main database header block - to be used once upon application initialization
void Controller::_WriteHeader(ofstream* stream) {
	/*
	database header is represented with 3 values
	version (char*)
	active model headers count (int)
	first model header address (streamoff)
	*/
	stream->seekp(ios::beg);
	stream->write(::version, utility::_CharSize(::version));
	header.modelCountPos = stream->tellp();
	stream->write(reinterpret_cast<char*>(&header.modelCount), sizeof(int));	
	header.lastHeaderPos = stream->tellp();
	stream->write(reinterpret_cast<char*>(&header.lastHeader), sizeof(streamoff));
	header.nextHeaderPos = stream->tellp();
	stream->write(reinterpret_cast<char*>(&header.nextHeader), sizeof(streamoff));
	header.nextHeader = header.nextHeaderPos + sizeof(streamoff);
}

//	adds new model header and edits main header information
void Controller::_AddNewModelHeader(ofstream* stream, ModelHeader& newHeader) {
	streamoff writePos = stream->tellp();
	stream->write(reinterpret_cast<char*>(&newHeader), sizeof(ModelHeader));

	header.lastHeader = writePos;
	stream->seekp(header.lastHeaderPos, ios::beg);
	stream->write(reinterpret_cast<char*>(&writePos), sizeof(streamoff));
	stream->seekp(header.modelCountPos, ios::beg);
	stream->write(reinterpret_cast<char*>(&++header.modelCount), sizeof(int));
}

//	get database header block
char* Controller::_GetHeader(ifstream* stream) {
	stream->seekg(ios::beg);
	//	database header is represented with 3 values
	//	char* = version, int = active model headers count, streamoff = first model header address
	int headerSize = 5 + sizeof(int) + sizeof(streamoff);
	char* header = new char[headerSize];
	stream->read(header, headerSize);
	return header;
}

//	validate application version
bool Controller::_ValidateVersion(ifstream* stream) {
	bool nuller = false;
	if (stream == nullptr) {
		stream = new ifstream;
		nuller = true;
	}	
	char* header = _GetHeader(stream);

	int versionSize = utility::_CharSize(::version);
	char* version = new char[versionSize];
	for (int i = 0; i < versionSize; i++)
		version[i] = *header++;
	bool result;
	if (utility::_CompareChar(version, ::version))
		result = true;		
	else result = false;
	header -= versionSize;

	delete[]version;
	delete[]header;

	if (nuller)
		delete stream;
	return result;
}

//	get count of active models headers in database
int Controller::_GetModelCount(ifstream* stream) {
	char* header = _GetHeader(stream);
	char* ptr = header;
	ptr += utility::_CharSize(::version);
	int count = *(int*)ptr;
	delete[]header;
	return count;	
}

//	get model header - each model has header which points to first node and contains count of model database records
char* Controller::_GetModelHeader(ifstream* stream, Model model) {
	int modelsCount = _GetModelCount(stream);
	for (int i = 0; i < modelsCount; i++) {
		//ModelHeader header = ModelHeader::_NextHeader(stream);


	}
	return nullptr;
}

char* Controller::_GetModels(ifstream* stream, Model model) {
	utility::LinkedList<char*> modelList;
	//stream->close();

	stream->seekg(header.nextHeader, ios::beg);
	char*buffer = new char[sizeof(ModelHeader)];
	stream->read(buffer, sizeof(ModelHeader));
	ModelHeader* newHeader = (ModelHeader*)buffer;

	int test = sizeof(ModelHeader);

	return nullptr;
}