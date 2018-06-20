#pragma once
#include "_Model.h"

class Header : public Model {
protected:	
	bool loaded = false;
	ModelName type = ModelName::none;
	int nodeCount = 0;
	int nextNodePage = -1;
	int lastNodePage = -1;
	std::streamoff pos = -1;	

	Header(ModelName type) : type(type){}
	
public:
	void _IncreaseNodeCount();
	void _EditNextNodePage(int pageNum);
	void _EditLastNodePage(int page);
	int _NextNodePage() const;
	int _LastNodePage() const;
	void _SetPosition(std::streamoff pos);
	std::streamoff _Position() const;
	ModelName _Type() const;
	bool _Loaded() const;
	void _SetLoadStatus(bool status);
};

inline void Header::_IncreaseNodeCount() {
	this->nodeCount++;
}

inline void Header::_EditNextNodePage(int pageNum) {
	this->nextNodePage = pageNum;
}

inline void Header::_EditLastNodePage(int pageNum) {
	this->lastNodePage = pageNum;
}

inline int Header::_NextNodePage() const {
	return this->nextNodePage;
}

inline int Header::_LastNodePage() const {
	return this->lastNodePage;
}

inline std::streamoff Header::_Position() const {
	return this->pos;
}

inline ModelName Header::_Type() const {
	return this->type;
}

inline void Header::_SetPosition(std::streamoff pos) {
	this->pos = pos;
}

inline bool Header::_Loaded() const {
	return this->loaded;
}

inline void Header::_SetLoadStatus(bool status) {
	this->loaded = status;
}

class MainHeader : public Header {
public:	
	char* _Serialize();
	void _Deserialize(char* page);

	MainHeader() : Header(ModelName::mainHeader) {}
	~MainHeader() = default;
};

class ModelHeader : public Header {
private:
	ModelName model = ModelName::none;
	
public:	
	ModelHeader(ModelName name) : Header(ModelName::modelHeader), model(name){}
	ModelHeader() : Header(ModelName::modelHeader) {}
	~ModelHeader() = default;

	char* _Serialize();
	void _Deserialize(char* page);
	ModelName _Name() const;
};

inline ModelName ModelHeader::_Name() const {
	return this->model;
}