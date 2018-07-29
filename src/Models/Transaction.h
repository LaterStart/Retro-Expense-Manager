#pragma once
#include "_Model.h"

class Transaction : public Model {
private:	
	int ID = -1;
	int profileID = -1;
	int accountID = -1;		
	int categoryID = -1;
	int currencyID = -1;
	int descriptionSize = 0;
	double amount = 0.0;
	char* description = nullptr;	
	char* amountChar = nullptr;
	TransactionType type = TransactionType::none;
	Date date;
	
	void _BindData(Data* data);
	std::ostream& _Show(std::ostream& os);
	void _AmountToChar();

public:	
	char* _Serialize();
	void _Deserialize(char* buffer);
	
	int _ID() const;	
	int _Currency() const;
	double _Amount() const;
	char* _AmountChar() const;
	char* _Description() const;
	int _Category() const;
	int _Account() const;
	int _DisplayLength();
	TransactionType _Type() const;
	Date _Date() override;

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
	return date._DisplayLength() + utility::_DigitNumberDouble(amount) + utility::_CharLength(description);
}

inline Date Transaction::_Date() {
	return this->date;
}

inline int Transaction::_Currency() const {
	return this->currencyID;
}

inline double Transaction::_Amount() const {
	return this->amount;
}

inline char* Transaction::_AmountChar() const {
	return this->amountChar;
}

inline char* Transaction::_Description() const {
	return this->description;
}

inline int Transaction::_Account() const {
	return this->accountID;
}

inline int Transaction::_Category() const {
	return this->categoryID;
}

inline TransactionType Transaction::_Type() const {
	return this->type;
}