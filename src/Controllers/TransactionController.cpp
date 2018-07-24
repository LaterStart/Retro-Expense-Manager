#include <fstream>
#include "TransactionController.h"
using namespace std;

//	transaction types
vector<const char*> TransactionController::transactionType{ "Income", "Expense", "Transfer", "Refund" };

//	static transaction model header
ModelHeader TransactionController::header(ModelName::transaction);

//	transaction controller constructor
TransactionController::TransactionController() {
	if (this->header._Loaded() == false) {
		_LoadHeader(this->header);
	}
	_LoadLatestTransactions();
}

//	Add new transaction
void TransactionController::_AddNewTransaction(utility::LinkedList<Data*>*data, int profileID) {
	Transaction newTransaction(data, header._GiveID(), profileID);
	fstream* stream = _OpenStream();

	// check if model header exists
	if (this->header._Loaded() == false)
		this->_WriteNewModelHeader(stream, this->header);

	// write model		
	char* buffer = newTransaction._Serialize();
	_WriteModel(stream, this->header, buffer);	

	// update last ten transactions vector
	if (latestTransactions->size() < 10)
		latestTransactions->push_back(newTransaction);
	else {
		latestTransactions->erase(latestTransactions->begin());
		latestTransactions->shrink_to_fit();
		latestTransactions->push_back(newTransaction);
	}
	
	stream->close();
	delete stream;
}

//	Loads last ten transactions
void TransactionController::_LoadLatestTransactions() {
	fstream* stream = _OpenStream();
	this->latestTransactions = new vector<Transaction>;
	if (stream != nullptr) {
		vector<char*>* buffer = _GetModels(stream, header, Query(Range::lastTen));
		if (buffer != nullptr) {
			for (size_t i = 0; i < buffer->size(); i++) {
				Transaction transaction(buffer->at(i));
				latestTransactions->push_back(transaction);
				delete[]buffer->at(i);
			}
			delete buffer;
		}
	}
	delete stream;
}

//	Returns vector with max 10 transactions
vector<Transaction>* TransactionController::_GetLatestTransactions() {
	return this->latestTransactions;
}