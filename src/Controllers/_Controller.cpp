#include <fstream>
#include "_Controller.h"
#include "../Models/_DataBlock.h"
using namespace std;

//	static header class instance - stores frequently accessed database data
MainHeader Controller::header;

//	load main haeder from database
void Controller::_LoadHeader() {
	fstream* stream = _OpenStream();
	if (stream != nullptr) {
		char* page = new char[clusterSize];
		do {
			streamoff pos = stream->tellg();
			stream->read(page, clusterSize);
			DataBlock block = _GetBlock(page, 0, ModelName::mainHeader);
			if (!block._Empty()) {
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

//	create fstream instance for binary write and read mode
fstream* Controller::_OpenStream() {
	fstream* stream = new fstream;
	stream->open(::database, ios::binary | ios::out | ios::in);
	if (stream->is_open())
		return stream;
	else {
		delete stream;
		return nullptr;
	}
}

//	restart stream - save data and create new stream
void Controller::_RestartStream(fstream* &stream) {
	stream->close();
	delete stream;
	stream = _OpenStream();
}

//	delete buffered data
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
	stream.open(::database);
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

		if (block._Model() == name && !block._Empty()) {			
			block._SetOffset(offset);
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
				if (!block._Empty()) {
					readPos = block._PagePos();
					readPos += block._NodeSize();
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
		} while (!stream->eof() && pageCount >= 0);
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
		streamoff pos = stream->tellp();
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

	// check if page has enough free space to write on
	streamoff pos = stream->tellp();
	streamoff pageNum = pos / clusterSize;
	streamoff writeSize = block._BlockSize() + block._BufferSize();
	streamoff freeSpace = (((pageNum + 1) * streamoff(clusterSize)) - pos);

	if (freeSpace - writeSize < 0) {
		// seek to next page
		char* emptyBuff = new char[(unsigned int)freeSpace];
		stream->write(emptyBuff, freeSpace);
		pos = stream->tellp();
	}

	stream->write(dblock, block._BlockSize());
	stream->write(buffer, block._BufferSize());
	header._SetPosition(pos);

	_DeleteBuffers(buffer, dblock, false);
	pageNum = pos / this->clusterSize;

	// update last model header node
	if (this->header._NodeCount() > 0)
		_UpdateLastNode(stream, this->header, ModelName::modelHeader, pageNum);

	// update main header
	this->header._IncreaseNodeCount();	
	if (this->header._FirstNode() < 0) 		
		this->header._EditFirstNode(pageNum);
	this->header._EditLastNode(pageNum);

	_UpdateHeader(stream, this->header);
	header._SetLoadStatus(true);
}

//	updates last node with new next node info
void Controller::_UpdateLastNode(fstream* stream, Header& header, ModelName name, streamoff pageNum) {	
	if (header._LastNode() < 0)
		return;
	stream->seekg(header._LastNode()*clusterSize, ios::beg);

	char* page = new char[clusterSize];
	stream->read(page, clusterSize);
	int readPos = 0;
	DataBlock block;
	bool update = false;
	do {
		block = _GetBlock(page, readPos, name);
		if (block._NextNode() < 0) {
			block._EditNextNode(pageNum, header._LastID());
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
	if (header._NodeCount() > 0)		
		block._EditPreviousNode(header._LastNode(), header._LastID() -1);
	char* dblock = block._Serialize();
		
	streamoff pos = stream->tellp();
	streamoff pageNum = pos / clusterSize;
	streamoff writeSize = block._BlockSize() + block._BufferSize();
	streamoff freeSpace = (((pageNum + 1) * streamoff(clusterSize)) - pos);

	if (freeSpace - writeSize < 0) {	
		char* emptyBuff = new char[(unsigned int)freeSpace];
		stream->write(emptyBuff, freeSpace);
		pos = stream->tellp();
	}
		
	stream->write(dblock, block._BlockSize());
	stream->write(buffer, block._BufferSize());

	_DeleteBuffers(buffer, dblock);
	
	pageNum = pos / clusterSize;
	
	if (header._NodeCount() > 0) 
		_UpdateLastNode(stream, header, header._Name(), pageNum);	

	header._IncreaseNodeCount();	
	if (header._FirstNode() < 0) {
		header._EditFirstNode(pageNum);
		header._SetFirstID(block._ID());
	}
	header._EditLastNode(pageNum);
	_UpdateHeader(stream, header);
}

//	returns all models that meet the query range conditions from database
vector<char*>* Controller::_GetModels(fstream* stream, ModelHeader& header, Query& query) {

	if (header._NodeCount() > 0 && header._FirstNode() >= 0) {

		char* page = new char[clusterSize];

		stream->seekg(header._FirstNode()*clusterSize, ios::beg);
		streamoff pos = stream->tellg();

		stream->read(page, clusterSize);
		streamoff pageNum = pos / clusterSize;

		int pagePos = 0;
		vector<char*>* buffer = new vector<char*>;
		int nextID = header._FirstID();

		for (unsigned int i = 0; i < header._NodeCount(); i++) {
			DataBlock block = _GetBlock(page, pagePos, header._Name());
			pagePos = block._PagePos();

			if (nextID != -2 && block._ID() != nextID) {
				pagePos += block._NodeSize();
				i--;
				continue;
			}
			nextID = block._NextID();		
			if (!block._Empty()) {
				int ID = block._ID();	
			
				bool acceptItem = query._ValidateID(ID, header);
				
				if (acceptItem) {						
					char* buff = new char[block._BufferSize() + sizeof(int)];
					std::memcpy(buff, &ID, sizeof(int));
					std::memcpy(buff + sizeof(int), page + block._Offset(), block._BufferSize());
				
					if (query._DateRange()) 
						acceptItem = query._ValidateDate(buff, header);
					if(acceptItem)						
						buffer->push_back(buff);
					else delete[]buff;
				}
				
				if (block._NextNode() != pageNum) {
					pagePos = 0;
					stream->clear();
					stream->seekg(block._NextNode() * clusterSize, ios::beg);
					pos = stream->tellg();
					stream->read(page, clusterSize);
					pageNum = pos / clusterSize;
				}				
				else if (block._NextNode() < 0)
					break;				
				else if (block._NextNode() == pageNum) 
					pagePos = 0;
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
			if (!block._Empty() && block._ID() == ID) {
				int ID = block._ID();
				buffer = new char[block._BufferSize() + sizeof(int)];
				std::memcpy(buffer, &ID, sizeof(int));
				std::memcpy(buffer+sizeof(int), (page + block._Offset()), block._BufferSize());
				break;
			}
			else if (!block._Empty() && block._NextNode() > pageNum) {
				pagePos = 0;
				stream->seekg(block._NextNode() * clusterSize, ios::beg);
				pos = stream->tellg();
				pageNum = pos / clusterSize;
				stream->read(page, clusterSize);
			}
			else if (block._NextNode() < 0)
				break;
			else pagePos += block._NodeSize();
		}

		stream->clear();
		delete[]page;
		return buffer;
	}
	else return nullptr;
}

//	Update existing model record with edited data
void Controller::_UpdateModel(fstream* &stream, ModelHeader& header, int ID, char* buffer) {
	if (header._NodeCount() > 0) {
		char* page = new char[clusterSize];
		stream->seekg(header._FirstNode()*clusterSize, ios::beg);
		streamoff pos = stream->tellg();
		stream->read(page, clusterSize);
		streamoff pageNum = pos / clusterSize;
		int pagePos = 0;

		int nextID = header._FirstID();
		for (unsigned int i = 0; i < header._NodeCount(); i++) {
			// find model with the given ID
			DataBlock originalBlock = _GetBlock(page, pagePos, header._Name());
			pagePos = originalBlock._PagePos();

			if (nextID != -2 && originalBlock._ID() != nextID) {
				pagePos += originalBlock._NodeSize();
				i--;
				continue;
			}
			nextID = originalBlock._NextID();
			if (!originalBlock._Empty() && originalBlock._ID() == ID) {
				DataBlock newBlock;
				newBlock._WrapData(buffer, header._Name());
				stream->clear();

				// if data size has not changed - rewrite existing record
				if (originalBlock._BufferSize() == newBlock._BufferSize()) {					
					stream->seekp(pageNum*clusterSize + originalBlock._Offset(), ios::beg);
					stream->write(buffer, originalBlock._BufferSize());
					buffer -= 2 * sizeof(int);
					delete[]buffer;
				}
				// if data size is different
				else {
					// update data block - set as empty
					originalBlock._SetEmpty(true);
					char* empty = originalBlock._Serialize();
					stream->seekp(pageNum*clusterSize + originalBlock._PagePos(), ios::beg);
					stream->write(empty, originalBlock._BlockSize());

					// update new block data
					newBlock._EditNextNode(originalBlock._NextNode(), originalBlock._NextID());
					newBlock._EditPreviousNode(originalBlock._PreviousNode(), originalBlock._PreviousID());
					char* dblock = newBlock._Serialize();

					// write edited data at the end of file
					stream->seekp(0, ios::end);

					// check if page has enough free space to write on
					streamoff pos = stream->tellp();
					streamoff pageNum = pos / clusterSize;
					streamoff writeSize = newBlock._BlockSize() + newBlock._BufferSize();
					streamoff freeSpace = (((pageNum + 1) * streamoff(clusterSize)) - pos);

					if (freeSpace - writeSize < 0) {
						// seek to next page
						char* emptyBuff = new char[(unsigned int)freeSpace];
						for (unsigned int i = 0; i < (unsigned int)freeSpace; i++) {
							emptyBuff = "0";
						}
						stream->write(emptyBuff, freeSpace);
						pos = stream->tellp();
					}

					stream->write(dblock, newBlock._BlockSize());
					stream->write(buffer, newBlock._BufferSize());

					_DeleteBuffers(buffer, dblock);
					pageNum = pos / clusterSize;	

					//	check if model is the first or last node - update header
					bool firstNode = (header._FirstID() == newBlock._ID()) ? true : false;
					bool lastNode = (header._LastID() == newBlock._ID()) ? true : false;
					if (firstNode) {
						header._EditFirstNode(pageNum);
						_RestartStream(stream);
						_UpdateHeader(stream, header);
					}
					else if (lastNode) {
						header._EditLastNode(pageNum);
						_RestartStream(stream);
						_UpdateHeader(stream, header);
					}

					// update previous node data
					if (newBlock._PreviousID() >= 0) {
						_RestartStream(stream);
						pagePos = 0;
						stream->seekg(newBlock._PreviousNode() * clusterSize, ios::beg);
						stream->read(page, clusterSize);

						do {
							// find model with the given ID
							DataBlock previousBlock = _GetBlock(page, pagePos, header._Name());
							pagePos = previousBlock._PagePos();
							if (!previousBlock._Empty() && previousBlock._ID() == newBlock._PreviousID()) {
								// update model data block
								previousBlock._EditNextNode(pageNum, previousBlock._NextID());
								char* pblock = previousBlock._Serialize();
								stream->clear();
								pageNum = newBlock._PreviousNode();
								stream->seekp(pageNum*clusterSize + previousBlock._PagePos(), ios::beg);
								stream->write(pblock, previousBlock._BlockSize());
								break;
							}
							else pagePos += previousBlock._NodeSize();
						} while (pagePos < clusterSize);
					}

					// update next node data
					if (newBlock._NextID() >= 0) {
						_RestartStream(stream);
						pagePos = 0;
						stream->seekg(newBlock._NextNode() * clusterSize, ios::beg);
						stream->read(page, clusterSize);

						do {
							// find model with the given ID
							DataBlock nextBlock = _GetBlock(page, pagePos, header._Name());
							pagePos = nextBlock._PagePos();
							if (!nextBlock._Empty() && nextBlock._ID() == newBlock._NextID()) {
								// update model data block
								nextBlock._EditPreviousNode(pageNum, nextBlock._PreviousID());
								char* pblock = nextBlock._Serialize();
								stream->clear();
								pageNum = newBlock._NextNode();
								stream->seekp(pageNum * clusterSize + nextBlock._PagePos(), ios::beg);
								stream->write(pblock, nextBlock._BlockSize());
								break;
							}
							else pagePos += nextBlock._NodeSize();
						} while (pagePos < clusterSize);
					}
				}				
				break;
			}
			// go to next node page
			if (originalBlock._NextNode() != pageNum) {
				pagePos = 0;
				stream->clear();
				stream->seekg(originalBlock._NextNode() * clusterSize, ios::beg);
				pos = stream->tellg();
				stream->read(page, clusterSize);
				pageNum = pos / clusterSize;
			}
			else if (originalBlock._NextNode() < 0)
				break;
			else if (originalBlock._NextNode() == pageNum)
				pagePos = 0;
			else pagePos += originalBlock._NodeSize();
		}
		delete[]page;
	}
}

bool Query::_ValidateID(int ID, ModelHeader& header) {
	if (this->range == Range::lastTen) {
		if (ID < header._LastID() - 9)
			return false;
	}
	if (includeIDs->size() > 0) {
		for (size_t i = 0; i < includeIDs->size(); i++) {
			if (includeIDs->at(i) == ID)
				return true;
		}
		return false;
	}
	else if (excludeIDs->size() > 0) {
		for (size_t i = 0; i < excludeIDs->size(); i++) {
			if (excludeIDs->at(i) == ID)
				return false;
		}
	}
	return true;
}

bool Query::_ValidateDate(char* buffer, ModelHeader& model) {
	Date date = model._GetModelDate(buffer);	
	Date currentDate(utility::_GetCurrentDate());
	switch (range) {
	case Range::thisMonth:
		if (date.year != currentDate.year)
			return false;
		if (date.month != currentDate.month)
			return false;
		break;
	default:
		break;
	}
	return true;
}