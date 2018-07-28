#pragma once
#include "../Controllers/_Controller.h"
#include "../utility.h"

class DataBlock {
private:
	static int blockSize;

	ModelName model = ModelName::none;
	int bufferSize = 0;
	int offset = -1;
	std::streamoff nextNode = - 1;
	std::streamoff previousNode = -1;
	int modelID = -1;
	int nextID = -1;
	int previousID = -1;
	bool empty = false;

public:	
	void _WrapData(char* &buffer, ModelName name, bool readID = true);	
	int _BufferSize() const;
	int _NodeSize() const;
	ModelName _Model() const;
	void _SetOffset(int pos);
	int _Offset() const;
	int _PagePos() const;
	std::streamoff _NextNode() const;
	void _EditNextNode(std::streamoff pageNum, int nextModelID);
	std::streamoff _PreviousNode() const;
	void _EditPreviousNode(std::streamoff pageNum, int previousModelID);
	int _ID() const;
	bool _Empty() const;
	void _SetEmpty(bool status);
	int _NextID() const;
	int _PreviousID() const;

	static int _BlockSize();
	
	char* _Serialize();
	void _Deserialize(char* page);
};

inline int DataBlock::_BlockSize(){
	return blockSize;
}

inline int DataBlock::_BufferSize() const {
	return this->bufferSize;
}

inline ModelName DataBlock::_Model() const {
	return this->model;
}

inline void DataBlock::_SetOffset(int pos) {
	this->offset = pos;
}

inline int DataBlock::_Offset() const {
	return this->offset + blockSize;
}

inline int DataBlock::_PagePos() const {
	return this->offset;
}

inline std::streamoff DataBlock::_NextNode() const {
	return this->nextNode;
}

inline void DataBlock::_EditNextNode(std::streamoff pageNum, int nextModelID) {
	this->nextNode = pageNum;
	this->nextID = nextModelID;
}

inline int DataBlock::_NodeSize() const {
	return blockSize + bufferSize;
}

inline int DataBlock::_ID() const {
	return this->modelID;
}

inline std::streamoff DataBlock::_PreviousNode() const {
	return this->previousNode;
}

inline void DataBlock::_EditPreviousNode(std::streamoff pageNum, int previousModelID) {
	this->previousNode = pageNum;
	this->previousID = previousModelID;
}

inline bool DataBlock::_Empty() const {
	return this->empty;
}

inline void DataBlock::_SetEmpty(bool status) {
	this->empty = status;
}

inline int DataBlock::_NextID() const {
	return this->nextID;
}

inline int DataBlock::_PreviousID() const {
	return this->previousID;
}
