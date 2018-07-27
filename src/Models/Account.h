#pragma once
#include "_Model.h"

class Account : public Model {
private:	
	int ID = -1;
	int profileID = -1;	
	int accountTypeID = -1;
	int nameSize = 0;
	int defaultCurrencyID = -1;
	char* name = nullptr;	
	bool multiCurrency = false;

	struct AccountBalance {
		int currencyID = -1;
		float totalAmount = 0.0;
	};
	
	std::vector<AccountBalance>* balance = nullptr;
	
	void _BindData(Data* data);
	std::ostream& _Show(std::ostream& os);

public:	
	char* _Serialize();
	void _Deserialize(char* buffer);	
	
	int _ID() const;
	char* _Name() const;
	int _Type() const;
	int _DefaultCurrency() const;
	int _DisplayLength();
	bool _MultiCurrency() const;

	Account() = default;
	Account(utility::LinkedList<Data*>* data, int ID, int profileID);
	Account(char* buffer);
	Account(const char* name, int accountTypeID);
	Account(const Account& copy);
	Account(Account&& move);
	~Account();

	Account& operator=(Account&& move);
};

inline int Account::_ID() const {
	return this->ID;
}

inline char* Account::_Name() const {
	return this->name;
}

inline int Account::_Type() const {
	return this->accountTypeID;
}

inline int Account::_DisplayLength() {
	return this-> nameSize;
}

inline int Account::_DefaultCurrency() const {
	return this->defaultCurrencyID;
}

inline bool Account::_MultiCurrency() const {
	return this->multiCurrency;
}