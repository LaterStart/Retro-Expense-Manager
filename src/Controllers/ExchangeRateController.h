#pragma once
#include "_Controller.h"
#include "../Models/ExchangeRate.h"

class ModelHeader;
class ExchangeRateController : public Controller {
private:
	static ModelHeader header;
	ExchangeRate* excRate;
	bool defaultSet = false;

	void _LoadExchangeRate();	
	void _ParseJSON(char* buffer);	

public:
	ExchangeRateController();
	~ExchangeRateController() = default;

	bool _DownloadExchangeRate();
	void _WriteExchangeRate();
	Currency* _GetCurrency(int id);
	void _SetDefaultCurrency(Currency* currency);
	double _ConvertCurrency(double amount, int fromID, int toID);

	std::vector<Currency>* currencies;		
};