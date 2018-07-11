#pragma once
#include "_Model.h"

class AccountType : public Model {
private:	
	int ID = -1;
	int nameSize = 0;
	char* name = nullptr;	
	
	void _BindData(Data* data);
	std::ostream& _Show(std::ostream& os);

public:	
	char* _Serialize();
	void _Deserialize(char* buffer);	
	
	int _ID() const;
	char* _Name() const;
	int _DisplayLength();

	AccountType() = default;
	AccountType(utility::LinkedList<Data*>* data, int ID);
	AccountType(char* buffer);
	AccountType(const char* name, int ID);
	AccountType(const AccountType& copy);
	~AccountType();
};

inline int AccountType::_ID() const {
	return this->ID;
}

inline char* AccountType::_Name() const {
	return this->name;
}

inline int AccountType::_DisplayLength() {
	return utility::_CharLength(name);
}