#include <fstream>
#include "AccountController.h"
using namespace std;

//	static account model header
ModelHeader AccountController::accountHeader(ModelName::account);

//	static account type model header
ModelHeader AccountController::accountTypeHeader(ModelName::accountType);

//	account controller constructor - loads accounts from database and creates default account types vector
AccountController::AccountController() {	
	if (accountHeader._Loaded() == false) {
		_LoadHeader(accountHeader);
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