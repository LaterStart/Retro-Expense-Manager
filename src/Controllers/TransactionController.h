#pragma once
#include "_Controller.h"
#include "../Models/Transaction.h"

class Transaction; class ModelHeader; 
class TransactionController : public Controller {
private:
	static ModelHeader header;	
	std::vector<Transaction>* latestTransactions = nullptr;

	void _LoadLatestTransactions();	

public:
	TransactionController();
	~TransactionController() = default;

	static std::vector<const char*> transactionType;
	
	void _AddNewTransaction(utility::LinkedList<Data*>*data, int profileID);
	std::vector<Transaction>* _GetLatestTransactions();	
	Transaction* _GetTransaction(int id);
	Transaction* _GetLastTransaction();
	std::vector<Transaction> _GetTransactions(Query query);
	Transaction* _DeserializeModel(char* buffer);
	int _LastTransactionID() const;
	void _EditTransaction(utility::LinkedList<Data*>*data, int modelID, int profileID);
};

inline int TransactionController::_LastTransactionID() const {
	return this->header._LastID();
}