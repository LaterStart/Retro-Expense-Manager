#include <fstream>
#include "AccountController.h"
#include "../Models/_Header.h"
#include "../Models/Account.h"
using namespace std;

//	account types
vector<const char*> AccountController::accountType{ "Cash", "Savings", "Credit Card", "Bank Account", "Loan", "Insurance", "Investment" };

//	static account model header
ModelHeader AccountController::header(ModelName::account);

//	account controller constructor - loads accounts from database
AccountController::AccountController() {
	this->model = ModelName::account;
	if (this->header._Loaded() == false) {
		_LoadHeader(this->header);
	}
	this->_LoadAccountList();
}

//	Add new account
void AccountController::_AddNewAccount(utility::LinkedList<Data*>*data, int profileID) {
	Account newAccount(data, header._GiveID(), profileID);
	fstream* stream = _OpenStream();

	// check if model header exists
	if(this->header._Loaded() == false)
		this->_WriteNewModelHeader(stream, this->header);

	// write model		
	char* buffer = newAccount._Serialize();
	_WriteModel(stream, this->header, buffer);	

	// update accounts list vector

	
	stream->close();
	delete stream;
}

//	Load accounts from database into 2D category list vector
void AccountController::_LoadAccountList() {

}