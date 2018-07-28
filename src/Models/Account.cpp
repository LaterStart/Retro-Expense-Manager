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
	
	balance = new std::vector<Balance>;
	if (multiCurrency == false) 
		balance->push_back(Balance(this->defaultCurrencyID));
}

// construct account model using bufferered data
Account::Account(char* buffer) {
	this->_Deserialize(buffer);
}

// construct new account model using account name
Account::Account(const char* name, int accountTypeID) {
	this->name = utility::_CopyChar(name);
	this->nameSize = utility::_CharSize(name);	
}

// account copy constructor
Account::Account(const Account& copy) {
	this->ID = copy.ID;
	this->profileID = copy.profileID;
	this->accountTypeID = copy.accountTypeID;
	this->name = utility::_CopyChar(copy.name);	
	this->nameSize = copy.nameSize;
	this->defaultCurrencyID = copy.defaultCurrencyID;
	this->multiCurrency = copy.multiCurrency;
	if(copy.balance != nullptr)
		this->balance = new std::vector<Balance>(*copy.balance);
}

// account move constructor
Account::Account(Account&& move) {
	this->ID = move.ID;
	this->profileID = move.profileID;
	this->accountTypeID = move.accountTypeID;
	this->name = move.name;
	this->nameSize = move.nameSize;
	move.name = nullptr;
	this->defaultCurrencyID = move.defaultCurrencyID;
	this->multiCurrency = move.multiCurrency;
	this->balance = move.balance;
	move.balance = nullptr;
}

// account move assignment
Account& Account::operator=(Account&& move) {
	if (&move == this)
		return *this;
	delete[]name;

	this->ID = move.ID;
	this->profileID = move.profileID;
	this->accountTypeID = move.accountTypeID;
	this->name = move.name;
	this->nameSize = move.nameSize;
	move.name = nullptr;
	this->defaultCurrencyID = move.defaultCurrencyID;
	this->multiCurrency = move.multiCurrency;
	this->balance = move.balance;
	move.balance = nullptr;

	return *this;
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
	case Field::currency:
		defaultCurrencyID = data->input->selection;
		break;
	case Field::multiCurrency:
		multiCurrency = data->input->check;
		break;
	default:
		break;
	}
}

//	serialize account model
char* Account::_Serialize() {
	//	Total object size					 
	int size = nameSize + 4*sizeof(int) + sizeof(bool);
	size += balance->size() * sizeof(Balance) + sizeof(int);

	//	insert object size info and ID at buffer start
	char* buffer = new char[size+2*sizeof(int)];
	char* firstByte = buffer;	

	std::memcpy(buffer, &ID, sizeof(int));
	buffer += sizeof(int);
	std::memcpy(buffer, &size, sizeof(int));	
	buffer += sizeof(int);

	//	store IDs into buffer
	int* ptr[] = { &profileID, &accountTypeID, &defaultCurrencyID};
	for (int i = 0; i < sizeof(ptr) / sizeof(ptr[0]); i++) {
		std::memcpy(buffer, *&ptr[i], sizeof(int));
		buffer += sizeof(int);
	}

	// store multi currency value into buffer
	std::memcpy(buffer, &multiCurrency, sizeof(bool));
	buffer += sizeof(bool);

	// store account balance into buffer
	int s = balance->size();
	std::memcpy(buffer, &s, sizeof(int));
	buffer += sizeof(int);
	for (size_t i = 0; i < balance->size(); i++) {
		std::memcpy(buffer, &balance->at(i), sizeof(Balance));
		buffer += sizeof(Balance);
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
	int* ptr[] = { &profileID, &accountTypeID, &defaultCurrencyID };
	for (int i = 0; i < sizeof(ptr) / sizeof(ptr[0]); i++) {
		*ptr[i] = *(int*)page;
		page += sizeof(int);
	}

	this->multiCurrency = *(bool*)page;
	page += sizeof(bool);

	//	deserialize account balance
	int s = *(int*)page;
	page += sizeof(int);
	this->balance = new std::vector<Balance>;
	for (int i = 0; i < s; i++) {
		Balance* instance = (Balance*)page;
		balance->push_back(*instance);
		page += sizeof(Balance);
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
	delete balance;
}

void Account::_UpdateBalance(double amount) {
	balance->at(0).totalAmount += amount;
}

void Account::_UpdateBalance(double amount, int currencyID) {
	bool found = false;
	for (size_t i = 0; i < balance->size(); i++) {
		if (balance->at(i).currencyID == currencyID) {
			balance->at(i).totalAmount += amount;
			found = true; 
			break;
		}
	}
	if (!found) {
		balance->push_back(Balance(currencyID));
		balance->back().totalAmount += amount;
	}	
}