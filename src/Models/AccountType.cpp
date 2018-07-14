#include "AccountType.h"
#include "../IO/Input.h"

// construct account type model using form data
AccountType::AccountType(utility::LinkedList<Data*>* data, int ID){
	while (true) {
		_BindData(data->element);
		if (data->nextNode == nullptr)
			break;
		else data = data->nextNode;
	}
	this->ID = ID;
	data->_DeleteList();
}

// construct account type model using bufferered data
AccountType::AccountType(char* buffer) {
	this->_Deserialize(buffer);
}

// construct new account type model using account type name
AccountType::AccountType(char* name, int ID) {
	this->ID = ID;
	this->name = utility::_CopyChar(name);
	this->nameSize = utility::_CharSize(name);
}

// account type copy constructor
AccountType::AccountType(const AccountType& copy) {
	this->ID = copy.ID;
	this->name = utility::_CopyChar(copy.name);
	this->nameSize = copy.nameSize;
}

// account type move constructor
AccountType::AccountType(AccountType&& move) {	
	this->ID = move.ID;
	this->name = move.name;
	this->nameSize = move.nameSize;
	move.name = nullptr;
}

// account type move assignment
AccountType& AccountType::operator=(AccountType&& move){	
	if (&move == this)
		return *this;
	delete[]name;

	this->ID = move.ID;
	this->name = move.name;
	this->nameSize = move.nameSize;
	move.name = nullptr;

	return *this;
}

//	binds form data to object data
void AccountType::_BindData(Data* data) {
	switch (data->field) {
	case Field::accountTypeName:
		name = utility::_CopyChar(data->input->input);
		nameSize = data->input->length + 1;
		break;
	default:
		break;
	}
}

//	serialize account type model
char* AccountType::_Serialize() {
	//	Total object size					 
	int size = nameSize + 3*sizeof(int);

	//	insert object size info and ID at buffer start
	char* buffer = new char[size+2*sizeof(int)];
	char* firstByte = buffer;	

	std::memcpy(buffer, &ID, sizeof(int));
	buffer += sizeof(int);
	std::memcpy(buffer, &size, sizeof(int));	
	buffer += sizeof(int);

	//	store name into buffer
	std::memcpy(buffer, &nameSize, sizeof(int));
	buffer += sizeof(int);
	std::memcpy(buffer, name, nameSize);

	return firstByte;
}

//	deserialize account model
void AccountType::_Deserialize(char* page) {
	this->ID = *(int*)page;
	page += sizeof(int);

	this->nameSize = *(int*)page;
	page += sizeof(int);

	this->name = new char[nameSize];
	std::memcpy(name, page, nameSize);
}

std::ostream& AccountType::_Show(std::ostream& os) {
	os << this->name;
	return os;
}

AccountType::~AccountType() {
	delete[]name;
}