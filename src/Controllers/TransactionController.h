#pragma once
#include "_Controller.h"

class Transaction; class ModelHeader;
class TransactionController : public Controller {
private:
	static ModelHeader header;
	

public:
	TransactionController();
	~TransactionController() = default;
	
	void _AddNewTransaction(utility::LinkedList<Data*>*data, int profileID);
};

extern std::vector<const char*> transactionType;