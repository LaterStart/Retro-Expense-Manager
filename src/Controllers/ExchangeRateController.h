#pragma once
#include "_Controller.h"
#include "../Models/ExchangeRate.h"

class ModelHeader;
class ExchangeRateController : public Controller {
private:
	static ModelHeader header;
	ExchangeRate* excRate;

	void _LoadExchangeRate();	
	void _ParseJSON(char* buffer);	

public:
	ExchangeRateController();
	~ExchangeRateController() = default;

	bool _DownloadExchangeRate();
	void _WriteExchangeRate();

	std::vector<Currency>* currencies;		
};