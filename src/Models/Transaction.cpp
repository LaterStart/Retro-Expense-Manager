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
	_AmountToChar();
}

// construct transacion model using bufferered data
Transaction::Transaction(char* buffer) {
	this->_Deserialize(buffer);
}

// transaction copy constructor
Transaction::Transaction(const Transaction& copy) {
	this->ID = copy.ID;
	this->profileID = copy.profileID;	
	this->accountID = copy.accountID;
	this->categoryID = copy.categoryID;
	this->currencyID = copy.currencyID;
	this->type = copy.type;
	this->amount = copy.amount;
	this->date = copy.date;
	this->descriptionSize = copy.descriptionSize;
	this->description = utility::_CopyChar(copy.description);
	this->amountChar = utility::_CopyChar(copy.amountChar);
	this->fromAccountID = copy.fromAccountID;

}

// transaction move constructor
Transaction::Transaction(Transaction&& move) {
	this->ID = move.ID;
	this->profileID = move.profileID;	
	this->accountID = move.accountID;
	this->categoryID = move.categoryID;
	this->currencyID = move.currencyID;
	this->type = move.type;
	this->amount = move.amount;
	this->date = move.date;
	this->descriptionSize = move.descriptionSize;
	this->description = move.description;
	move.description = nullptr;
	this->amountChar = move.amountChar;
	move.amountChar = nullptr;
	this->fromAccountID = move.fromAccountID;
}

// transaction move assignment
Transaction& Transaction::operator=(Transaction&& move) {
	if (&move == this)
		return *this;
	delete[]description;

	this->ID = move.ID;
	this->profileID = move.profileID;	
	this->accountID = move.accountID;
	this->categoryID = move.categoryID;
	this->currencyID = move.currencyID;
	this->type = move.type;
	this->amount = move.amount;
	this->date = move.date;
	this->descriptionSize = move.descriptionSize;
	this->description = move.description;
	move.description = nullptr;
	this->amountChar = move.amountChar;
	move.amountChar = nullptr;
	this->fromAccountID = move.fromAccountID;

	return *this;
}

//	binds form data to object data
void Transaction::_BindData(Data* data) {
	switch (data->field) {
	case Field::account:
		if (accountID > -1) 
			fromAccountID = accountID;
		accountID = data->input->selection;		
		break;
	case Field::transactionType:
		type = static_cast<TransactionType>(data->input->selection);
		break;
	case Field::category:
		categoryID = data->input->selection;
		break;
	case Field::currency:
		currencyID = data->input->selection;
		break;
	case Field::amount:
		amount = utility::_ConvertToDouble(data->input->input);
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

//	serialize transaction model
char* Transaction::_Serialize() {
	//	Total object size					 
	int size = descriptionSize + 7*sizeof(int) + sizeof(double) + sizeof(Date);

	//	insert object size info and ID at buffer start
	char* buffer = new char[size+2*sizeof(int)];
	char* firstByte = buffer;	

	std::memcpy(buffer, &ID, sizeof(int));
	buffer += sizeof(int);
	std::memcpy(buffer, &size, sizeof(int));	
	buffer += sizeof(int);

	//	store IDs into buffer
	int* ptr[] = { &profileID, &accountID, &categoryID, &currencyID, &fromAccountID };
	for (int i = 0; i < sizeof(ptr) / sizeof(ptr[0]); i++) {
		std::memcpy(buffer, *&ptr[i], sizeof(int));
		buffer += sizeof(int);
	}

	//	store type into buffer
	std::memcpy(buffer, &type, sizeof(int));
	buffer += sizeof(int);

	//	store amount into buffer
	std::memcpy(buffer, &amount, sizeof(double));
	buffer += sizeof(double);

	//	store description into buffer
	std::memcpy(buffer, &descriptionSize, sizeof(int));
	buffer += sizeof(int);
	std::memcpy(buffer, description, descriptionSize);
	buffer += descriptionSize;

	//	store date into buffer
	std::memcpy(buffer, &date, sizeof(Date));

	return firstByte;
}

//	deserialize transaction model
void Transaction::_Deserialize(char* page) {
	this->ID = *(int*)page;
	page += sizeof(int);

	//	deserialize IDs
	int* ptr[] = { &profileID, &accountID, &categoryID, &currencyID, &fromAccountID };
	for (int i = 0; i < sizeof(ptr) / sizeof(ptr[0]); i++) {
		*ptr[i] = *(int*)page;
		page += sizeof(int);
	}

	//	deserialize type
	type = *(TransactionType*)page;
	page += sizeof(int);

	this->amount = *(double*)page;
	page += sizeof(double);

	this->descriptionSize = *(int*)page;
	page += sizeof(int);

	this->description = new char[descriptionSize];
	std::memcpy(description, page, descriptionSize);
	page += descriptionSize;

	this->date = *(Date*)page;
	_AmountToChar();
}

std::ostream& Transaction::_Show(std::ostream& os) {
	os << this->date<<'/'<<this->amount<<'/'<<this->description;
	return os;
}

Transaction::~Transaction() {
	delete[]description;
	delete[]amountChar;
}

void Transaction::_AmountToChar() {
	int size = utility::_DigitNumberDouble(amount) + 3;
	this->amountChar = new char[size];
	switch (type) {
	case TransactionType::income:
	case TransactionType::refund:
		amountChar[0] = '+';
		break;
	case TransactionType::expense:
		amountChar[0] = '-';
		break;
	case TransactionType::transfer:
		amountChar[0] = '=';
		break;
	default:
		break;
	}
	char* tmp = utility::_DoubleToChar(amount);
	char* tmp2 = amountChar + 1;
	std::memcpy(tmp2, tmp, utility::_CharSize(tmp));
}