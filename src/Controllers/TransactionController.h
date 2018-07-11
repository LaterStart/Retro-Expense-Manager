#pragma once
#include "_Controller.h"
#include "../Models/Transaction.h"

class Transaction; class ModelHeader;
class TransactionController : public Controller {
private:
	static ModelHeader header;
	

public:
	TransactionController();
	~TransactionController() = default;

	static std::vector<const char*> transactionType;
	
	void _AddNewTransaction(utility::LinkedList<Data*>*data, int profileID);
};