#include <fstream>
#include "TransactionController.h"
using namespace std;

//	transaction types
vector<const char*> TransactionController::transactionType{ "Income", "Expense", "Transfer", "Refund" };

//	static transaction model header
ModelHeader TransactionController::header(ModelName::transaction);

//	transaction controller constructor
TransactionController::TransactionController() {}

//	Add new transaction
void TransactionController::_AddNewTransaction(utility::LinkedList<Data*>*data, int profileID) {
	Transaction newTransaction(data, header._GiveID(), profileID);
	fstream* stream = _OpenStream();

	// write model		
	char* buffer = newTransaction._Serialize();
	_WriteModel(stream, this->header, buffer);	
	
	stream->close();
	delete stream;
}