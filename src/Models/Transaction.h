#pragma once
#include "_Model.h"

class Transaction : public Model {
private:	
	int ID = -1;
	int profileID = -1;
	int accountID = -1;	
	int typeID = -1;
	int categoryID = -1;
	int currencyID = -1;
	int descriptionSize = 0;
	float amount = 0.0;
	char* description = nullptr;	
	Date date;
	
	void _BindData(Data* data);
	std::ostream& _Show(std::ostream& os);

public:	
	char* _Serialize();
	void _Deserialize(char* buffer);
	
	int _ID() const;
	Date _Date() const;
	int _DisplayLength();

	Transaction(utility::LinkedList<Data*>* data, int ID, int profileID);
	Transaction(char* buffer);
	Transaction(const Transaction& copy);
	Transaction(Transaction&& move);
	~Transaction();

	Transaction& operator=(Transaction&& move);
};

inline int Transaction::_ID() const {
	return this->ID;
}

inline int Transaction::_DisplayLength() {
	return date._DisplayLength() + utility::_DigitNumber(amount) + utility::_CharLength(description);
}

inline Date Transaction::_Date() const {
	return this->date;
}