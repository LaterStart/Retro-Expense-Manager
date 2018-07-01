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
	int modelID = -1;

public:
	bool empty = true;
	void _WrapData(char* &buffer, ModelName name, bool readID = true);	
	int _BufferSize() const;
	int _NodeSize() const;
	ModelName _Model() const;
	void _SetOffset(int pos);
	int _Offset() const;
	int _PagePos() const;
	std::streamoff _NextNode() const;
	void _EditNextNode(std::streamoff pageNum);
	int _ID() const;

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

inline void DataBlock::_EditNextNode(std::streamoff pageNum) {
	this->nextNode = pageNum;
}

inline int DataBlock::_NodeSize() const {
	return blockSize + bufferSize;
}

inline int DataBlock::_ID() const {
	return this->modelID;
}