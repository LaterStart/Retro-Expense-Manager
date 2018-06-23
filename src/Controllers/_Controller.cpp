#include <fstream>
#include "_Controller.h"
#include "../Models/Header.h"
#include "../Models/DataBlock.h"
using namespace std;

//	static header class instance - stores frequently accessed database data
MainHeader Controller::header;

//	load main haeder from database
Controller::Controller(bool& initialize) {
	if (!header._Loaded()) {
		_LoadHeader();
		initialize = true;
	}
}

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

void Controller::_DeleteBuffers(char* &buffer, char* &dblock, bool containsID) {
	buffer -= sizeof(int);
	if (containsID)
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
		block._WrapData(buffer, ModelName::mainHeader, false);

		//write data block
		char* dblock = block._Serialize();
		streamoff pos = stream.tellp();
		stream.write(dblock, block._BlockSize());
		stream.write(buffer, block._BufferSize());
		header._SetPosition(pos);
		
		stream.close();
		_DeleteBuffers(buffer, dblock, false);
		return true;
	}
	return false;
}

//	finds requested block name on the given page buffer
DataBlock Controller::_GetBlock(char* page, int offset,  ModelName name) {	
	page += offset;
	do {
		DataBlock block;
		block._Deserialize(page);
		if (block._Model() == name) {
			block._SetOffset(offset);
			block.empty = false;
			return block;
		}
		else {
			offset += block._NodeSize();
			page += block._NodeSize();
		}
	} while (offset < clusterSize);
	DataBlock empty;
	return empty;
}

//	load main header from database
void Controller::_LoadHeader() {
	fstream* stream = _OpenStream();	
	if (stream != nullptr) {
		char* page = new char[clusterSize];
		do {
			streamoff pos = stream->tellg();
			stream->read(page, clusterSize);
			DataBlock block = _GetBlock(page, 0, ModelName::mainHeader);			
			if (!block.empty) {
				page += block._Offset();
				header._Deserialize(page);
				header._SetPosition(pos + block._Offset() - block._BlockSize());
				header._SetLoadStatus(true);
				page -= block._Offset();				
				break;
			}	
		} while (!stream->eof());		
		stream->close();
		delete[]page;
	}	
	delete stream;
}

//	load requested model header from database
void Controller::_LoadHeader(ModelHeader& header) {
	fstream* stream = _OpenStream();
	if (stream != nullptr) {
		streamoff pageCount = 0;
		char* page = new char[clusterSize];
		do {
			streamoff pos = stream->tellg();
			stream->read(page, clusterSize);
			int readPos = 0;
			do {
				DataBlock block = _GetBlock(page, readPos, ModelName::modelHeader);
				if (!block.empty) {
					readPos += block._PagePos();
					ModelHeader temporary;					
					temporary._Deserialize(page+block._Offset());					

					if (temporary._Name() == header._Name()) {
						temporary._SetPosition(pageCount*clusterSize + block._PagePos());
						header = temporary;
						header._SetLoadStatus(true);
						stream->close();
						delete stream;
						delete[]page;
						return;
					}
					else if (block._NextNode() != pageCount) {
						pageCount = block._NextNode() - 1;
						stream->seekg(clusterSize*block._NextNode(), ios::beg);
						break;
					}
				}
				else break;
			} while (readPos < clusterSize);			
			pageCount++;
		} while (!stream->eof());
		stream->close();
		delete[]page;
	}
	delete stream;
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
		int bufferSize = *(int*)buffer;	
		stream->seekp(header._Position() + DataBlock::_BlockSize(), ios::beg);
		stream->write(buffer + sizeof(int), bufferSize);
		delete[]buffer;
	}
}

//	writes new model header into database
void Controller::_WriteNewModelHeader(fstream* stream, ModelHeader& header) {
	stream->seekp(0, ios::end);
	char* buffer = header._Serialize();

	DataBlock block;
	block._WrapData(buffer, ModelName::modelHeader, false);
	char* dblock = block._Serialize();

	streamoff pos = stream->tellp();
	stream->write(dblock, block._BlockSize());
	stream->write(buffer, block._BufferSize());
	header._SetPosition(pos);

	_DeleteBuffers(buffer, dblock, false);
	streamoff pageNum = pos / this->clusterSize;

	// update last model header node
	if (header._NodeCount() > 0)
		_UpdateLastNode(stream, this->header, ModelName::modelHeader, pageNum);

	// update main header
	this->header._IncreaseNodeCount();	
	if (this->header._FirstNode() < 0) 		
		this->header._EditFirstNode(pageNum);
	this->header._EditLastNode(pageNum);

	_UpdateHeader(stream, this->header);
}

//	updates last node with new next node info
void Controller::_UpdateLastNode(fstream* stream, Header& header, ModelName name, streamoff pageNum) {	
	stream->seekg(header._LastNode()*clusterSize, ios::beg);

	char* page = new char[clusterSize];
	stream->read(page, clusterSize);
	int readPos = 0;
	DataBlock block;
	bool update = false;
	do {
		block = _GetBlock(page, readPos, name);
		if (block._NextNode() < 0) {
			block._EditNextNode(pageNum);
			update = true;
			break;
		}
		readPos = block._PagePos();
		readPos += block._NodeSize() ;
	} while (readPos < clusterSize);
	delete[]page;
	stream->clear();

	if (update) {
		char* dblock = block._Serialize();
		stream->seekp((header._LastNode()*clusterSize + block._PagePos()), ios::beg);		
		stream->write(dblock, block._BlockSize());
		delete[]dblock;
	}
}

//	write new model data into database
void Controller::_WriteModel(fstream* stream, ModelHeader& header, char* buffer) {
	stream->seekp(0, ios::end);

	DataBlock block;
	block._WrapData(buffer, header._Name());
	char* dblock = block._Serialize();

	streamoff pos = stream->tellp();
	stream->write(dblock, block._BlockSize());
	stream->write(buffer, block._BufferSize());

	_DeleteBuffers(buffer, dblock);
	streamoff pageNum = pos / clusterSize;

	// update last model node block
	if (header._NodeCount() > 0) 
		_UpdateLastNode(stream, header, header._Name(), pageNum);

	// update model header
	header._IncreaseNodeCount();	
	if (header._FirstNode() < 0)
		header._EditFirstNode(pageNum);
	header._EditLastNode(pageNum);

	_UpdateHeader(stream, header);
}

//	returns all models that meet the query range conditions from database
char** Controller::_GetModels(fstream* stream, ModelHeader& header, Query query) {
	if (header._NodeCount() > 0) {
		char* page = new char[clusterSize];
		stream->seekg(header._FirstNode()*clusterSize, ios::beg);
		streamoff pos = stream->tellg();
		stream->read(page, clusterSize);
		streamoff pageNum = pos / clusterSize;
		int pagePos = 0;

		char**buffer = new char*[header._NodeCount()];
		for (unsigned int i = 0; i < header._NodeCount(); i++) {
			DataBlock block = _GetBlock(page, pagePos, header._Name());
			pagePos = block._PagePos();
			if (!block.empty) {
				int ID = block._ID();
				buffer[i] = new char[block._BufferSize() + sizeof(int)];
				std::memcpy(buffer[i], &ID, sizeof(int));
				std::memcpy(buffer[i]+sizeof(int), page + block._Offset(), block._BufferSize());

				if (block._NextNode() > pageNum) {
					pagePos = 0;
					stream->seekg(block._NextNode() * clusterSize, ios::beg);
					stream->read(page, clusterSize);
				}
				else pagePos += block._NodeSize();
			}
		}

		stream->clear();
		delete[]page;
		return buffer;
	}
	else return nullptr;
}

//	returns model with the given ID from database
char* Controller::_GetModel(fstream* stream, ModelHeader& header, int ID) {
	if (header._NodeCount() > 0) {
		char* page = new char[clusterSize];
		stream->seekg(header._FirstNode()*clusterSize, ios::beg);
		streamoff pos = stream->tellg();
		stream->read(page, clusterSize);
		streamoff pageNum = pos / clusterSize;
		int pagePos = 0;
		char* buffer = nullptr;

		for (unsigned int i = 0; i < header._NodeCount(); i++) {
			DataBlock block = _GetBlock(page, pagePos, header._Name());
			pagePos = block._PagePos();
			if (!block.empty && block._ID() == ID) {
				int ID = block._ID();
				buffer = new char[block._BufferSize() + sizeof(int)];
				std::memcpy(buffer, &ID, sizeof(int));
				std::memcpy(buffer+sizeof(int), (page + block._Offset()), block._BufferSize());
				break;
			}
			else if (!block.empty && block._NextNode() > pageNum) {
				pagePos = 0;
				stream->seekg(block._NextNode() * clusterSize, ios::beg);
				stream->read(page, clusterSize);
			}
			else pagePos += block._NodeSize();
		}

		stream->clear();
		delete[]page;
		return buffer;
	}
	else return nullptr;
}

void Controller::_UpdateModel(fstream* stream, ModelHeader& header, int ID, char* buffer) {
	if (header._NodeCount() > 0) {
		char* page = new char[clusterSize];
		stream->seekg(header._FirstNode()*clusterSize, ios::beg);
		streamoff pos = stream->tellg();
		stream->read(page, clusterSize);
		streamoff pageNum = pos / clusterSize;
		int pagePos = 0;

		for (unsigned int i = 0; i < header._NodeCount(); i++) {
			DataBlock originalBlock = _GetBlock(page, pagePos, header._Name());
			pagePos = originalBlock._PagePos();
			if (!originalBlock.empty && originalBlock._ID() == ID) {
				DataBlock newBlock;
				newBlock._WrapData(buffer, header._Name());

				if (originalBlock._BufferSize() == newBlock._BufferSize()) {
					stream->seekp(pageNum*clusterSize + originalBlock._Offset(), ios::beg);
					stream->write(buffer, originalBlock._BufferSize());
					buffer -= sizeof(int);
					delete[]buffer;
				}
				else {



				}

				break;
			}
			else if (!originalBlock.empty && originalBlock._NextNode() > pageNum) {
				pagePos = 0;
				stream->seekg(originalBlock._NextNode() * clusterSize, ios::beg);
				stream->read(page, clusterSize);
			}
			else pagePos += originalBlock._NodeSize();
		}

		stream->clear();
		delete[]page;
	}
}