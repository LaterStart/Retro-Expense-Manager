#include "ExchangeRate.h"
#include "../IO/Input.h"

// construct exchange rate model using bufferered data
ExchangeRate::ExchangeRate(char* buffer) {
	this->_Deserialize(buffer);
}

// exchange rate copy constructor
ExchangeRate::ExchangeRate(const ExchangeRate& copy) {
	this->ID = copy.ID;
	this->baseID = copy.baseID;
	this->ccyNum = copy.ccyNum;
	this->date = copy.date;
}

// exchange rate move constructor
ExchangeRate::ExchangeRate(ExchangeRate&& move) {
	this->ID = move.ID;
	this->baseID = move.baseID;
	this->ccyNum = move.ccyNum;
	this->date = move.date;
}

// exchange rate move assignment
ExchangeRate& ExchangeRate::operator=(ExchangeRate&& move) {
	if (&move == this)
		return *this;

	this->ID = move.ID;
	this->baseID = move.baseID;
	this->ccyNum = move.ccyNum;
	this->date = move.date;

	return *this;
}

//	serialize exchange rate model
char* ExchangeRate::_Serialize() {
	//	Total object size					 
	int size = 2 * sizeof(int) + sizeof(Date);

	//	insert object size info and ID at buffer start
	char* buffer = new char[size + 2 * sizeof(int)];
	char* firstByte = buffer;

	std::memcpy(buffer, &ID, sizeof(int));
	buffer += sizeof(int);
	std::memcpy(buffer, &size, sizeof(int));
	buffer += sizeof(int);

	//	store base ccy id into buffer;
	std::memcpy(buffer, &baseID, sizeof(int));
	buffer += sizeof(int);

	//	store currency number into buffer
	std::memcpy(buffer, &ccyNum, sizeof(int));
	buffer += sizeof(int);

	//	store exchange rate date into buffer
	std::memcpy(buffer, &date, sizeof(Date));

	return firstByte;
}

//	deserialize exchange rate model
void ExchangeRate::_Deserialize(char* page) {
	this->ID = *(int*)page;
	page += sizeof(int);

	this->baseID = *(int*)page;
	page += sizeof(int);

	this->ccyNum = *(int*)page;
	page += sizeof(int);

	this->date = *(Date*)page;
}

std::ostream& ExchangeRate::_Show(std::ostream& os) {
	os << this->date;
	return os;
}

// construct currency model using bufferered data
Currency::Currency(char* buffer) {
	this->_Deserialize(buffer);
}

// currency copy constructor
Currency::Currency(const Currency& copy) {
	this->ID = copy.ID;
	this->name = utility::_CopyChar(copy.name);
	this->nameSize = copy.nameSize;
	this->rate = copy.rate;
}

// currency move constructor
Currency::Currency(Currency&& move) {
	this->ID = move.ID;
	this->name = move.name;
	this->nameSize = move.nameSize;
	this->rate = move.rate;
	move.name = nullptr;
}

// currency move assignment
Currency& Currency::operator=(Currency&& move) {
	if (&move == this)
		return *this;
	delete[]name;

	this->ID = move.ID;
	this->name = move.name;
	this->nameSize = move.nameSize;
	this->rate = move.rate;
	move.name = nullptr;

	return *this;
}

//	serialize currency model
char* Currency::_Serialize() {
	//	Total object size					 
	int size = nameSize + 3*sizeof(int) + sizeof(float);

	//	insert object size info and ID at buffer start
	char* buffer = new char[size+2*sizeof(int)];
	char* firstByte = buffer;	

	std::memcpy(buffer, &ID, sizeof(int));
	buffer += sizeof(int);
	std::memcpy(buffer, &size, sizeof(int));	
	buffer += sizeof(int);

	//	store rate into buffer;
	std::memcpy(buffer, &rate, sizeof(float));
	buffer += sizeof(float);

	//	store name into buffer
	std::memcpy(buffer, &nameSize, sizeof(int));
	buffer += sizeof(int);
	std::memcpy(buffer, name, nameSize);

	return firstByte;
}

//	deserialize currency model
void Currency::_Deserialize(char* page) {
	this->ID = *(int*)page;
	page += sizeof(int);

	this->rate = *(float*)page;
	page += sizeof(float);

	this->nameSize = *(int*)page;
	page += sizeof(int);

	this->name = new char[nameSize];
	std::memcpy(name, page, nameSize);
}

std::ostream& Currency::_Show(std::ostream& os) {
	os << this->name;
	return os;
}

Currency::~Currency() {
	delete[]name;
}