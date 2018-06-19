#pragma once
#include "_Model.h"

class DataBlock : public Model {
private:
	ModelName type = ModelName::none;
	int blockSize = 0;
	int bufferSize = 0;
	int offset = -1;

public:
	bool empty = true;
	void _WrapData(char* &buffer, ModelName name);
	int _BlockSize() const;
	int _BufferSize() const;
	ModelName _Type() const;
	void _SetOffset(int pos);
	int _Offset() const;
	
	char* _Serialize();
	void _Deserialize(char* page);
};

inline int DataBlock::_BlockSize() const {
	return blockSize;
}

inline int DataBlock::_BufferSize() const {
	return bufferSize;
}

inline ModelName DataBlock::_Type() const {
	return type;
}

inline void DataBlock::_SetOffset(int pos) {
	this->offset = pos;
}

inline int DataBlock::_Offset() const {
	return this->offset + blockSize;
}