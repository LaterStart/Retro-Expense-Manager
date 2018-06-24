#include "Profile.h"
#include "../IO/Input.h" //dbg

// construct profile model using form data
Profile::Profile(utility::LinkedList<Data*>* data, int ID){
	while (true) {
		_BindData(data->element);
		if (data->nextNode == nullptr)
			break;
		else data = data->nextNode;
	}
	this->ID = ID;
	data->_DeleteList();
}

// construct profile model using buffer
Profile::Profile(char* buffer) {
	this->_Deserialize(buffer);
}

// profile copy constructor
Profile::Profile(const Profile& copy) {
	this->ID = copy.ID;
	this->active = copy.active;
	this->defCCYid = copy.defCCYid;
	this->pwProtected = copy.pwProtected;
	this->usernameSize = copy.usernameSize;
	this->passwordSize = copy.passwordSize;
	this->password = utility::_CopyChar(copy.password);
	this->username = utility::_CopyChar(copy.username);
}

//	binds form data to object data
void Profile::_BindData(Data* data) {
	switch (data->field) {
	case Field::username:
		username = data->input->input;
		usernameSize = data->input->length + 1;
		break;
	case Field::pwStatus:
		pwProtected = data->input->check;
		break;
	case Field::password:
		password = data->input->input;
		passwordSize = data->input->length + 1;
		break;
	case Field::defCCYid:
		defCCYid = data->input->selection;
		break;
	default:
		break;
	}
}

//	serialize profile model
char* Profile::_Serialize() {
	//	Total object size					  pw status   username size   pw size       def ccy id   active status
	int size = usernameSize + passwordSize + sizeof(bool) + sizeof(int) + sizeof(int) + sizeof(int) + sizeof(bool);

	//	insert object size info and ID at buffer start
	char* buffer = new char[size+2*sizeof(int)];
	char* firstByte = buffer;	

	std::memcpy(buffer, &ID, sizeof(int));
	buffer += sizeof(int);
	std::memcpy(buffer, &size, sizeof(int));	
	buffer += sizeof(int);

	//	store username into buffer
	std::memcpy(buffer, &usernameSize, sizeof(int));
	buffer += sizeof(int);
	std::memcpy(buffer, username, usernameSize);
	buffer += usernameSize;

	//	store password protection status into buffer
	std::memcpy(buffer, &pwProtected, sizeof(bool));
	buffer += sizeof(bool);

	//	store password into buffer
	std::memcpy(buffer, &passwordSize, sizeof(int));
	buffer += sizeof(int);
	if (pwProtected) {
		utility::_XOR(password);
		std::memcpy(buffer, password, passwordSize);
		buffer += passwordSize;
	}

	// store default currency ID into buffer
	std::memcpy(buffer, &defCCYid, sizeof(int));
	buffer += sizeof(int);

	std::memcpy(buffer, &active, sizeof(bool));

	return firstByte;
}

//	deserialize profile model
void Profile::_Deserialize(char* page) {
	this->ID = *(int*)page;
	page += sizeof(int);

	this->usernameSize = *(int*)page;
	page += sizeof(int);

	this->username = new char[usernameSize];
	std::memcpy(username, page, usernameSize);
	page += usernameSize;

	this->pwProtected = *(bool*)page;
	page += sizeof(bool);

	this->passwordSize = *(int*)page;
	page += sizeof(int);

	if (pwProtected) {
		this->password = new char[passwordSize];
		std::memcpy(password, page, passwordSize);
		page += passwordSize;
	}		

	this->defCCYid = *(int*)page;
	page += sizeof(int);
	
	this->active = *(bool*)page;
}

//	verify if the given password is correct
bool Profile::_VerifyPassword(char* password) {
	utility::_XOR(this->password);
	bool verify = utility::_CompareChar(this->password, password);
	utility::_XOR(this->password);
	return verify;
}

Profile::~Profile() {
	delete[]username;
	delete[]password;
}