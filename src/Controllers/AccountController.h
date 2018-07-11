#pragma once
#include "_Controller.h"
#include "../Models/Account.h"
#include "../Models/AccountType.h"

class Account; class AccountType;  class ModelHeader;
class AccountController : public Controller {
private:
	static ModelHeader accountHeader;	
	static ModelHeader accountTypeHeader;

public:
	AccountController();
	~AccountController() = default;

	std::vector<AccountType>* accountTypes;
	std::vector<Account>* accounts;
	
	void _AddNewAccount(utility::LinkedList<Data*>*data, int profileID);
	void _AddNewAccountType(utility::LinkedList<Data*>*data);
	void _LoadAccounts();
	void _LoadAccountTypes();
};