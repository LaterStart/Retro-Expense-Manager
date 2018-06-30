#include "Transaction.h"
#include "../IO/Input.h"

// construct transaction model using form data
Transaction::Transaction(utility::LinkedList<Data*>* data, int ID, int profileID){
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

// construct transacion model using bufferered data
Transaction::Transaction(char* buffer) {
	this->_Deserialize(buffer);
}

// transaction copy constructor
Transaction::Transaction(const Transaction& copy) {

}

//	binds form data to object data
void Transaction::_BindData(Data* data) {
	switch (data->field) {
	case Field::account:
		accountID = data->input->selection;		
		break;
	case Field::transactionType:
		typeID = data->input->selection;
		break;
	case Field::category:
		categoryID = data->input->selection;
		break;
	case Field::currency:
		currencyID = data->input->selection;
		break;
	case Field::amount:
		amount = utility::_ConvertToFloat(data->input->input);
		break;
	case Field::description:
		description = data->input->input;
		descriptionSize = data->input->length;
	default:
		break;
	}
}

//	serialize profile model
char* Transaction::_Serialize() {
	//	Total object size					 
	int size = descriptionSize + 6*sizeof(int) + sizeof(float);

	//	insert object size info and ID at buffer start
	char* buffer = new char[size+2*sizeof(int)];
	char* firstByte = buffer;	

	std::memcpy(buffer, &ID, sizeof(int));
	buffer += sizeof(int);
	std::memcpy(buffer, &size, sizeof(int));	
	buffer += sizeof(int);

	//	store IDs into buffer
	int* ptr[] = { &profileID, &accountID, &typeID, &categoryID, &currencyID };
	for (int i = 0; i < 5; i++) {
		std::memcpy(buffer, *&ptr[i], sizeof(int));
		buffer += sizeof(int);
	}

	//	store amount into buffer
	std::memcpy(buffer, &amount, sizeof(float));
	buffer += sizeof(float);

	//	store description into buffer
	std::memcpy(buffer, &descriptionSize, sizeof(int));
	buffer += sizeof(int);
	std::memcpy(buffer, description, descriptionSize);

	return firstByte;
}

//	deserialize profile model
void Transaction::_Deserialize(char* page) {
	this->ID = *(int*)page;
	page += sizeof(int);

	//	deserialize IDs
	int* ptr[] = { &profileID, &accountID, &typeID, &categoryID, &currencyID };
	for (int i = 0; i < 5; i++) {
		*ptr[i] = *(int*)page;
		page += sizeof(int);
	}

	this->amount = *(float*)page;
	page += sizeof(float);

	this->descriptionSize = *(int*)page;
	page += sizeof(int);

	this->description = new char[descriptionSize];
	std::memcpy(description, page, descriptionSize);
}

Transaction::~Transaction() {
	delete[]description;
}