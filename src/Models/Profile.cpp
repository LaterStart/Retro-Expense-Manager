#include "Profile.h"
#include "../IO/Input.h" //dbg

// construct profile model using form data
Profile::Profile(utility::LinkedList<Data*>* data){
	while (data != nullptr) {
		_BindData(data->element);
		data = data->nextNode;
	}
}

// construct profile model using buffer
Profile::Profile(char* buffer) {
	this->_Deserialize(buffer);
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
	//	Total object size					//pw status   username size   pw size       def ccy id    
	int size = usernameSize + passwordSize + sizeof(bool) + sizeof(int) + sizeof(int) + sizeof(int);
	//	insert object size info at buffer start
	char* buffer = new char[size+sizeof(int)];
	char* firstByte = buffer;
	std::memcpy(buffer, &size, sizeof(int));	
	buffer += sizeof(int);

	//	store username into buffer
	std::memcpy(buffer, &usernameSize, sizeof(int));
	buffer += sizeof(int);
	for (int i = 0; i < usernameSize; i++)
		*buffer++ = *username++;

	//	store password protection status into buffer
	std::memcpy(buffer, &pwProtected, sizeof(bool));
	buffer += sizeof(bool);

	//	store password into buffer
	std::memcpy(buffer, &passwordSize, sizeof(int));
	buffer += sizeof(int);
	if (pwProtected) {
		utility::_XOR(password);
		for (int i = 0; i < passwordSize; i++)
			*buffer++ = *password++;
	}

	// store default currency ID into buffer
	std::memcpy(buffer, &defCCYid, sizeof(int));

	return firstByte;
}

//	deserialize profile model
void Profile::_Deserialize(char* page) {
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
}