#include "Account.h"
#include "../IO/Input.h"

// construct account model using form data
Account::Account(utility::LinkedList<Data*>* data, int ID, int profileID){
	while (true) {
		_BindData(data->element);
		if (data->nextNode == nullptr)
			break;
		else data = data->nextNode;
	}
	this->ID = ID;
	this->profileID = profileID;
	data->_DeleteList();
}

// construct account model using bufferered data
Account::Account(char* buffer) {
	this->_Deserialize(buffer);
}

// construct new account model using account name
Account::Account(const char* name, int accountTypeID) {
	this->name = utility::_CopyChar(name);
	this->accountTypeID = accountTypeID;
}

// account copy constructor
Account::Account(const Account& copy) {
	this->ID = copy.ID;
	this->profileID = copy.profileID;
	this->accountTypeID = copy.accountTypeID;
	this->name = utility::_CopyChar(copy.name);	
}

//	binds form data to object data
void Account::_BindData(Data* data) {
	switch (data->field) {
	case Field::accountName:
		name = utility::_CopyChar(data->input->input);
		nameSize = data->input->length + 1;
		break;
	case Field::accountType:
		accountTypeID = data->input->selection;
		break;
	default:
		break;
	}
}

//	serialize account model
char* Account::_Serialize() {
	//	Total object size					 
	int size = nameSize + 3*sizeof(int);

	//	insert object size info and ID at buffer start
	char* buffer = new char[size+2*sizeof(int)];
	char* firstByte = buffer;	

	std::memcpy(buffer, &ID, sizeof(int));
	buffer += sizeof(int);
	std::memcpy(buffer, &size, sizeof(int));	
	buffer += sizeof(int);

	//	store IDs into buffer
	int* ptr[] = { &profileID, &accountTypeID };
	for (int i = 0; i < sizeof(ptr) / sizeof(ptr[0]); i++) {
		std::memcpy(buffer, *&ptr[i], sizeof(int));
		buffer += sizeof(int);
	}

	//	store name into buffer
	std::memcpy(buffer, &nameSize, sizeof(int));
	buffer += sizeof(int);
	std::memcpy(buffer, name, nameSize);

	return firstByte;
}

//	deserialize account model
void Account::_Deserialize(char* page) {
	this->ID = *(int*)page;
	page += sizeof(int);

	//	deserialize IDs
	int* ptr[] = { &profileID, &accountTypeID };
	for (int i = 0; i < sizeof(ptr) / sizeof(ptr[0]); i++) {
		*ptr[i] = *(int*)page;
		page += sizeof(int);
	}
	
	this->nameSize = *(int*)page;
	page += sizeof(int);

	this->name = new char[nameSize];
	std::memcpy(name, page, nameSize);
}

std::ostream& Account::_Show(std::ostream& os) {
	os << this->name;
	return os;
}

Account::~Account() {
	delete[]name;
}