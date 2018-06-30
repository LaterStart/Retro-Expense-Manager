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
	
	void _BindData(Data* data);

public:	
	char* _Serialize();
	void _Deserialize(char* buffer);
	
	int _ID() const;

	Transaction(utility::LinkedList<Data*>* data, int ID, int profileID);
	Transaction(char* buffer);
	Transaction(const Transaction& copy);
	~Transaction();
};

inline int Transaction::_ID() const {
	return this->ID;
}