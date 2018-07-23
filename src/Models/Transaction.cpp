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
	this->ID = copy.ID;
	this->profileID = copy.profileID;
	this->typeID = copy.typeID;
	this->accountID = copy.accountID;
	this->categoryID = copy.categoryID;
	this->currencyID = copy.currencyID;
	this->amount = copy.amount;
	this->date = copy.date;
	this->descriptionSize = copy.descriptionSize;
	this->description = utility::_CopyChar(copy.description);
}

// transaction move constructor
Transaction::Transaction(Transaction&& move) {
	this->ID = move.ID;
	this->profileID = move.profileID;
	this->typeID = move.typeID;
	this->accountID = move.accountID;
	this->categoryID = move.categoryID;
	this->currencyID = move.currencyID;
	this->amount = move.amount;
	this->date = move.date;
	this->descriptionSize = move.descriptionSize;
	this->description = move.description;
	move.description = nullptr;
}

// transaction move assignment
Transaction& Transaction::operator=(Transaction&& move) {
	if (&move == this)
		return *this;
	delete[]description;

	this->ID = move.ID;
	this->profileID = move.profileID;
	this->typeID = move.typeID;
	this->accountID = move.accountID;
	this->categoryID = move.categoryID;
	this->currencyID = move.currencyID;
	this->amount = move.amount;
	this->date = move.date;
	this->descriptionSize = move.descriptionSize;
	this->description = move.description;
	move.description = nullptr;

	return *this;
}

//	binds form data to object data
void Transaction::_BindData(Data* data) {
	switch (data->field) {
	case Field::account:
		accountID = data->input->selection -1;		
		break;
	case Field::transactionType:
		typeID = data->input->selection -1;
		break;
	case Field::category:
		categoryID = data->input->selection -1;
		break;
	case Field::currency:
		currencyID = data->input->selection;
		break;
	case Field::amount:
		amount = utility::_ConvertToFloat(data->input->input);
		break;
	case Field::description:
		description = utility::_CopyChar(data->input->input);
		descriptionSize = data->input->length + 1;
		break;
	case Field::date:
		date = Date(data->input->input);
		break;
	default:
		break;
	}
}

//	serialize category model
char* Transaction::_Serialize() {
	//	Total object size					 
	int size = descriptionSize + 6*sizeof(int) + sizeof(float) + sizeof(Date);

	//	insert object size info and ID at buffer start
	char* buffer = new char[size+2*sizeof(int)];
	char* firstByte = buffer;	

	std::memcpy(buffer, &ID, sizeof(int));
	buffer += sizeof(int);
	std::memcpy(buffer, &size, sizeof(int));	
	buffer += sizeof(int);

	//	store IDs into buffer
	int* ptr[] = { &profileID, &accountID, &typeID, &categoryID, &currencyID };
	for (int i = 0; i < sizeof(ptr) / sizeof(ptr[0]); i++) {
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
	buffer += descriptionSize;

	//	store date into buffer
	std::memcpy(buffer, &date, sizeof(Date));

	return firstByte;
}

//	deserialize category model
void Transaction::_Deserialize(char* page) {
	this->ID = *(int*)page;
	page += sizeof(int);

	//	deserialize IDs
	int* ptr[] = { &profileID, &accountID, &typeID, &categoryID, &currencyID };
	for (int i = 0; i < sizeof(ptr) / sizeof(ptr[0]); i++) {
		*ptr[i] = *(int*)page;
		page += sizeof(int);
	}

	this->amount = *(float*)page;
	page += sizeof(float);

	this->descriptionSize = *(int*)page;
	page += sizeof(int);

	this->description = new char[descriptionSize];
	std::memcpy(description, page, descriptionSize);
	page += descriptionSize;

	this->date = *(Date*)page;
}

std::ostream& Transaction::_Show(std::ostream& os) {
	os << this->date<<'/'<<this->amount<<'/'<<this->description;
	return os;
}

Transaction::~Transaction() {
	delete[]description;
}