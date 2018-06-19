#include "Profile.h"
#include "../IO/Input.h" //dbg

Profile::Profile(utility::LinkedList<Data*>* data){
	while (data != nullptr) {
		_BindData(data->element);
		data = data->nextNode;
	}
}

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

char* Profile::_Serialize() {
	//	Total object size
	int size = usernameSize + passwordSize + sizeof(bool) + (2 * sizeof(int));
	//	insert object size info at buffer start
	char* buffer = new char[size+sizeof(int)];
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
	buffer -= (size + sizeof(int));

	return buffer;
}

void Profile::_Deserialize(char* page) {

}