#pragma once
#include "../Controllers/_Controller.h"
#include "../utility.h"

class Header {
protected:	
	int ID = 0;
	bool loaded = false;
	ModelName type = ModelName::none;
	unsigned int nodeCount = 0;
	unsigned int idCount = 0;
	std::streamoff firstNode = -1;
	std::streamoff lastNode = -1;
	std::streamoff pos = -1;	

	Header(ModelName type) : type(type){}
	
public:
	void _IncreaseNodeCount();
	void _EditFirstNode(std::streamoff pageNum);
	void _EditLastNode(std::streamoff page);
	std::streamoff _FirstNode() const;
	std::streamoff _LastNode() const;
	void _SetPosition(std::streamoff pos);
	std::streamoff _Position() const;
	ModelName _Type() const;
	bool _Loaded() const;
	void _SetLoadStatus(bool status);
	unsigned int _NodeCount() const;
	unsigned int _GiveID();
	void _SetNodeCount(int nodeCount);
	void _SetIDCount(int idCount);
	void _SetStartID(int startID);
	int _LastID() const;

	virtual ~Header() = default;
};

inline void Header::_IncreaseNodeCount() {
	this->nodeCount++;
}

inline void Header::_EditFirstNode(std::streamoff pageNum) {
	this->firstNode = pageNum;
}

inline void Header::_EditLastNode(std::streamoff pageNum) {
	this->lastNode = pageNum;
}

inline std::streamoff Header::_FirstNode() const {
	return this->firstNode;
}

inline std::streamoff Header::_LastNode() const {
	return this->lastNode;
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

inline unsigned int Header::_NodeCount() const {
	return this->nodeCount;
}

inline unsigned int Header::_GiveID(){
	return this->idCount++;
}

inline void Header::_SetNodeCount(int nodeCount) {
	this->nodeCount = nodeCount;
}

inline void Header::_SetStartID(int startID) {
	this->idCount = startID;
}

inline int Header::_LastID() const {
	return this->idCount - 1;
}

inline void Header::_SetIDCount(int idCount) {
	this->idCount = idCount;
}

class MainHeader : public Header {
public:	
	char* _Serialize();
	void _Deserialize(char* page);

	MainHeader() : Header(ModelName::mainHeader) {}
	~MainHeader() = default;
};

class Controller;
class ModelHeader : public Header{
private:
	ModelName model = ModelName::none;	
	Controller* controller = nullptr;
	int firstID = -1;
	
public:	
	ModelHeader(ModelName name) : Header(ModelName::modelHeader), model(name){}
	ModelHeader() : Header(ModelName::modelHeader) {}
	~ModelHeader() = default;

	char* _Serialize();
	void _Deserialize(char* page);
	ModelName _Name() const;
	void _ResetIDCounter();	
	Date _GetModelDate(char* buffer);
	void _LinkController(Controller* controller);
	int _FirstID() const;
	void _SetFirstID(int firstID);
};

inline ModelName ModelHeader::_Name() const {
	return this->model;
}

inline void ModelHeader::_ResetIDCounter() {
	this->idCount = 0;
}

inline void ModelHeader::_LinkController(Controller* controller) {
	this->controller = controller;
}

inline int ModelHeader::_FirstID() const {
	return this->firstID;
}

inline void ModelHeader::_SetFirstID(int firstID) {
	this->firstID = firstID;
}