#include <fstream>
#include "AccountController.h"
#include "../IO/Input.h"
using namespace std;

//	static account model header
ModelHeader AccountController::accountHeader(ModelName::account);

//	static account type model header
ModelHeader AccountController::accountTypeHeader(ModelName::accountType);

//	account controller constructor - loads accounts from database and creates default account types vector
AccountController::AccountController() {	
	if (accountHeader._Loaded() == false) {
		_LoadHeader(accountHeader);
		accountHeader._LinkController(this);
		accountTypeHeader._LinkController(this);
	}
	_LoadAccounts();
	accountTypes = new vector<AccountType>{ 
		AccountType("Cash", 0),
		AccountType("Credit Card", 1),
		AccountType("Bank Account", 2),
		AccountType("Insurance", 3),
		AccountType("Loan", 4),
		AccountType("Investment", 5)
	};
	accountTypeHeader._SetNodeCount(6);
	accountTypeHeader._SetStartID(6);
	if (accountTypeHeader._Loaded() == false) {
		_LoadHeader(accountTypeHeader);
	}
	_LoadAccountTypes();
}

//	Add new account
void AccountController::_AddNewAccount(utility::LinkedList<Data*>*data, int profileID) {
	Account newAccount(data, accountHeader._GiveID(), profileID);
	fstream* stream = _OpenStream();

	// check if model header exists
	if(accountHeader._Loaded() == false)
		_WriteNewModelHeader(stream, accountHeader);

	// write model		
	char* buffer = newAccount._Serialize();
	_WriteModel(stream, accountHeader, buffer);	

	// update accounts list vector
	accounts->push_back(newAccount);
	
	stream->close();
	delete stream;
}

//	Add new account type
void AccountController::_AddNewAccountType(utility::LinkedList<Data*>* data) {
	AccountType newAccountType(data, accountTypeHeader._GiveID());
	fstream* stream = _OpenStream();

	// check if model header exists
	if (accountTypeHeader._Loaded() == false)
		_WriteNewModelHeader(stream, accountTypeHeader);

	// write model		
	char* buffer = newAccountType._Serialize();
	_WriteModel(stream, accountTypeHeader, buffer);

	// update account types list vector
	accountTypes->push_back(newAccountType);

	stream->close();
	delete stream;
}

void AccountController::_AddNewAccountType(char* name) {
	AccountType newAccountType(name, accountTypeHeader._GiveID());
	fstream* stream = _OpenStream();

	// check if model header exists
	if (accountTypeHeader._Loaded() == false)
		_WriteNewModelHeader(stream, accountTypeHeader);

	// write model		
	char* buffer = newAccountType._Serialize();
	_WriteModel(stream, accountTypeHeader, buffer);

	// update account types list vector
	accountTypes->push_back(newAccountType);

	stream->close();
	delete stream;
}

//	Load accounts from database into 1D accounts vector
void AccountController::_LoadAccounts() {
	accounts = new vector<Account>;
	fstream* stream = _OpenStream();
	if (stream != nullptr) {
		vector<char*>* buffer = _GetModels(stream, this->accountHeader, Query(Range::all));
		if (buffer != nullptr) {
			for (unsigned int i = 0; i < buffer->size(); i++) {
				Account account(buffer->at(i));
				accounts->push_back(account);
				delete[]buffer->at(i);
			}
			delete buffer;
		}
		stream->close();
	}
	delete stream;
}

//	Load account types from database into 1D account types vector
void AccountController::_LoadAccountTypes() {
	fstream* stream = _OpenStream();
	if (stream != nullptr) {
		vector<char*>* buffer = _GetModels(stream, this->accountTypeHeader, Query(Range::all));
		if (buffer != nullptr) {
			for (unsigned int i = 0; i < buffer->size(); i++) {
				AccountType accountType(buffer->at(i));
				accountTypes->push_back(accountType);
				delete[]buffer->at(i);
			}
			delete buffer;
		}
		stream->close();
	}
	delete stream;
}

//	check if account with the given name already exists
bool AccountController::_Exists(char* name) {
	fstream* stream = _OpenStream();
	bool result = false;
	if (stream != nullptr) {
		vector<char*>* buffer = _GetModels(stream, this->accountHeader, Query(Range::all));
		if (buffer != nullptr) {
			for (unsigned int i = 0; i < buffer->size(); i++) {
				Account temporary(buffer->at(i));
				delete[]buffer->at(i);
				if (utility::_CompareChar(temporary._Name(), name)) {
					result = true;
					break;
				}
			}
			delete buffer;
		}
		stream->close();
	}
	delete stream;
	return result;
}

//	Returns account with the given id
Account* AccountController::_GetAccount(int id) {
	for (size_t i = 0; i < accounts->size(); i++) {
		if (accounts->at(i)._ID() == id)
			return &accounts->at(i);
	}
	return nullptr;
}

//	Updates account database record
void AccountController::_UpdateAccount(Account* account) {
	fstream* stream = _OpenStream();
	if (stream != nullptr) {
		char* buffer = account->_Serialize();
		_UpdateModel(stream, accountHeader, account->_ID(), buffer);
		stream->close();
	}
	delete stream;
}

Account* AccountController::_DeserializeModel(char* buffer) {
	Account* account = new Account(buffer);
	return account;
}
