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
	void _ChangeBase(Currency* currency);

public:
	ExchangeRateController();
	~ExchangeRateController() = default;

	bool _DownloadExchangeRate();
	void _WriteExchangeRate();
	Currency* _GetCurrency(int id);
	void _SetDefaultCurrency(Currency* currency);
	double _ConvertCurrency(double amount, int fromID, int toID);
	Model* _DeserializeModel(char* buffer);
	
	std::vector<Currency>* currencies;		
};