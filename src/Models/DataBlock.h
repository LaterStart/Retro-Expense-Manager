#pragma once
#include "_Model.h"

class DataBlock : public Model {
private:
	ModelName type = ModelName::none;
	int blockSize = 0;
	int bufferSize = 0;
	int offset = -1;
	std::streamoff nextNode = - 1;

public:
	bool empty = true;
	void _WrapData(char* &buffer, ModelName name);
	int _BlockSize() const;
	int _BufferSize() const;
	int _NodeSize() const;
	ModelName _Type() const;
	void _SetOffset(int pos);
	int _Offset() const;
	int _PagePos() const;
	std::streamoff _NextNode() const;
	void _EditNextNode(std::streamoff pageNum);
	
	char* _Serialize();
	void _Deserialize(char* page);
};

inline int DataBlock::_BlockSize() const {
	return this->blockSize;
}

inline int DataBlock::_BufferSize() const {
	return this->bufferSize;
}

inline ModelName DataBlock::_Type() const {
	return this->type;
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