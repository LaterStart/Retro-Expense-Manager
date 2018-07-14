#pragma once
#include "_Model.h"

class ExchangeRate : public Model {
private:
	int ID = -1;
	Date date;
	int baseID = -1;
	int ccyNum = 0;

	std::ostream& _Show(std::ostream& os);

public:
	char* _Serialize();
	void _Deserialize(char* buffer);

	int _DisplayLength();
	int _CurrencyNumber() const;
	void _SetBaseID(int ccyID);
	void _SetCurrencyNumber(int num);
	void _SetDate(Date date);
	Date _Date() const;

	ExchangeRate() = default;
	ExchangeRate(char* buffer);
	ExchangeRate(const ExchangeRate& copy);
	ExchangeRate(ExchangeRate&& move);
	~ExchangeRate() = default;

	ExchangeRate& operator=(ExchangeRate&& move);
};

inline int ExchangeRate::_DisplayLength() {
	return this->date._DisplayLength();
}

inline void ExchangeRate::_SetBaseID(int ccyID) {
	this->baseID = ccyID;
}

inline void ExchangeRate::_SetCurrencyNumber(int num) {
	this->ccyNum = num;
}

inline void ExchangeRate::_SetDate(Date date) {
	this->date = date;
}

inline int ExchangeRate::_CurrencyNumber() const {
	return this->ccyNum;
}

inline Date ExchangeRate::_Date() const {
	return this->date;
}

class Currency: public ExchangeRate {
private:	
	int ID = -1;
	int nameSize = 0;
	char* name = nullptr;	
	float rate = 0.0;	

	std::ostream& _Show(std::ostream& os);

public:	
	char* _Serialize() override;
	void _Deserialize(char* buffer) override;	
	int _DisplayLength() override;
	
	int _ID() const;
	char* _Name() const;	
	void _SetRate(float rate);
	void _SetID(int ID);
	void _SetName(char* name);

	Currency() = default;
	Currency(char* buffer);
	Currency(const Currency& copy);
	Currency(Currency&& move);
	~Currency();

	Currency& operator=(Currency&& move);
};

inline int Currency::_ID() const {
	return this->ID;
}

inline char* Currency::_Name() const {
	return this->name;
}

inline int Currency::_DisplayLength() {
	return nameSize;
}

inline void Currency::_SetRate(float rate) {
	this->rate = rate;
}

inline void Currency::_SetID(int ID) {
	this->ID = ID;
}

inline void Currency::_SetName(char* name) {
	this->name = utility::_CopyChar(name);
	this->nameSize = utility::_CharSize(name);
}