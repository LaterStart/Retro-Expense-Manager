#pragma once
#include "_Controller.h"

class Account; class ModelHeader;
class AccountController : public Controller {
private:
	static ModelHeader header;	

public:
	AccountController();
	~AccountController() = default;

	static std::vector<const char*> accountType;
	std::vector<std::vector<Account>>* accountsList;
	
	void _AddNewAccount(utility::LinkedList<Data*>*data, int profileID);
	void _LoadAccountList();
};