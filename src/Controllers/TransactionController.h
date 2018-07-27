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
};