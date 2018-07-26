#include <fstream>
#include <winsock.h>
#include <winhttp.h>
#include "ExchangeRateController.h"
using namespace std;

//	static exchange rate model header
ModelHeader ExchangeRateController::header(ModelName::exchangeRate);

//	exchange rate controller constructor - loads exchange rate from database
ExchangeRateController::ExchangeRateController() {
	if (header._Loaded() == false)
		_LoadHeader(header);

	_LoadExchangeRate();	
}

//	Load exchange rate from database into 1D currency vector
void ExchangeRateController::_LoadExchangeRate() {
	currencies = new vector<Currency>;

	fstream* stream = _OpenStream();
	if (stream != nullptr) {
		// load current exchange rate info
		if (header._Loaded() == true) {
			excRate = new ExchangeRate(_GetModel(stream, header, -1));

			// check if current excRate is older 
			Date date(utility::_GetCurrentDate());
			if (excRate->_Date() < date) {
				ExchangeRate old(*excRate);
				// Download new exchange rate
				this->header._ResetIDCounter();
				_DownloadExchangeRate();

				if (excRate->_CurrencyNumber() == old._CurrencyNumber()) {
					// update existing exchange rate records
					char* buffer = excRate->_Serialize();
					_UpdateModel(stream, header, -1, buffer);
					stream->clear();
					for (size_t i = 0; i < currencies->size(); i++) {
						buffer = currencies->at(i)._Serialize();
						_UpdateModel(stream, header, currencies->at(i)._ID(), buffer);
					}
				}
				else {
					int test = 0;



				}
			}
			else {
				// load current exchange rate
				Query query(Range::all);
				query._ExcludeIDs(-1);
				vector<char*>* buffer = _GetModels(stream, this->header, query);
				if (buffer != nullptr) {
					for (unsigned int i = 0; i < buffer->size(); i++) {
						Currency currency(buffer->at(i));
						currencies->push_back(currency);
						delete[]buffer->at(i);
					}
					delete buffer;
				}				
			}
		}
		else {
			// if no exchange rate data was found - initiate download and save records into database
			_DownloadExchangeRate();
			_WriteExchangeRate();
		}
		stream->close();
	}
	delete stream;
}

//	Download exchange rate JSON
bool ExchangeRateController::_DownloadExchangeRate() {	
	//return false;
	DWORD dwSize = 0;
	DWORD dwDownloaded = 0;
	BOOL  bResults = FALSE;
	HINTERNET  hSession = NULL,
		hConnect = NULL,
		hRequest = NULL;

	// Use WinHttpOpen to obtain a session handle.
	hSession = WinHttpOpen(L"Exchange Rates API - JSON",
		WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
		WINHTTP_NO_PROXY_NAME,
		WINHTTP_NO_PROXY_BYPASS, 0);

	// Specify an HTTP server.
	if (hSession)
		hConnect = WinHttpConnect(hSession, L"data.fixer.io",
			INTERNET_DEFAULT_HTTP_PORT, 0);

	// Create an HTTP request handle.
	if (hConnect)
		hRequest = WinHttpOpenRequest(hConnect, L"GET", L"/api/latest?access_key=08155c857d63ed1fb44bb92906bb7d14",
			NULL, WINHTTP_NO_REFERER,
			NULL,
			NULL);

	// Send a request.
	if (hRequest)
		bResults = WinHttpSendRequest(hRequest,
			WINHTTP_NO_ADDITIONAL_HEADERS, 0,
			WINHTTP_NO_REQUEST_DATA, 0,
			0, 0);


	// End the request.
	if (bResults)
		bResults = WinHttpReceiveResponse(hRequest, NULL);

	// Keep checking for data until there is nothing left.
	char* buffer = nullptr;
	Buffer* node = new Buffer;
	Buffer* next = nullptr;
	Buffer* first = node;

	int size = 0;
	if (bResults) {
		do {
			// Check for available data.
			dwSize = 0;
			WinHttpQueryDataAvailable(hRequest, &dwSize);

			// Read the data.
			if (dwSize > 0) {
				buffer = new char[dwSize];
				if (WinHttpReadData(hRequest, (LPVOID)buffer,
					dwSize, &dwDownloaded)) {

					node->data = buffer;
					node->size = dwSize;
					size += node->size;

					next = new Buffer;
					node->nextNode = next;
					node = next;
				}
			}
		} while (dwSize > 0);
	}
	bool returnVal = false;
	if (first->data != nullptr) {
		node = first;
		char* bufferRes = new char[size];
		int z, x = 0;
		while (node->nextNode != nullptr) {
			z = 0;
			while (z < node->size)
				bufferRes[x++] = node->data[z++];
			Buffer* deleter = node;			
			node = node->nextNode;
			delete deleter;
		}
		_ParseJSON(bufferRes);
		delete[]bufferRes;
		returnVal = true;
	}

	// Close any open handles.
	if (hRequest) WinHttpCloseHandle(hRequest);
	if (hConnect) WinHttpCloseHandle(hConnect);
	if (hSession) WinHttpCloseHandle(hSession);

	return returnVal;
}

//	Parse downloaded JSON and store data into vector
void ExchangeRateController::_ParseJSON(char* buffer) {
	bool status;
	delete excRate;
	excRate = new ExchangeRate;

	int pos = 0, cpos = 0;
	pos = utility::_Find("success", buffer);
	if (buffer[pos += 9] == 't')
		status = true;
	else return;

	pos = utility::_Find("base", buffer) + 7;
	char* baseName = new char[4];
	for (int i = 0; i < 3; i++)
		baseName[i] = buffer[pos + i];
	baseName[3] = '\0';

	pos = utility::_Find("date", buffer) + 7;
	Date date;
	char* tmp = new char[4];
	while (cpos < 4)
		tmp[cpos++] = buffer[pos++];
	date.year = utility::_ConvertToInteger(tmp,4);
	cpos = 0; pos++;
	while (cpos < 2)
		tmp[cpos++] = buffer[pos++];
	date.month = utility::_ConvertToInteger(tmp,2);
	cpos = 0; pos++;
	while (cpos < 2)
		tmp[cpos++] = buffer[pos++];
	date.day = utility::_ConvertToInteger(tmp,2);
	delete[]tmp;

	excRate->_SetDate(date);

	pos = utility::_Find("rates", buffer) + 9;
	int z;

	do {
		z = 0;
		Currency ccy;
		char ccyName[4];
		ccyName[3] = '\0';
		while (buffer[pos] != '\"')
			ccyName[z++] = buffer[pos++];

		ccy._SetName(ccyName);

		if (excRate->_BaseID() < 0 && strcmp(ccyName, baseName) == 0) {
			this->excRate->_SetBaseID(currencies->size());
			delete[]baseName;
		}

		pos += 2; z = 0;

		char* rateBuffer; int mem = pos;
		while (buffer[mem] != ',' && buffer[mem] != '}') {
			z++;
			mem++;
		}
		rateBuffer = new char[z + 1];
		rateBuffer[z] = '\0';
		z = 0;
		while (buffer[pos] != ',' && buffer[pos] != '}')
			rateBuffer[z++] = buffer[pos++];
		ccy._SetRate((float)atof(rateBuffer));
		delete[]rateBuffer;

		ccy._SetID(header._GiveID());
		currencies->push_back(ccy);

		pos++;
		if (buffer[pos] == '}')
			break;
		else pos++;
	} while (true);
	excRate->_SetCurrencyNumber(currencies->size());
}

//	Writes downloaded exchange rate data into database - should have only one instance of these records (always update/rewrite)
void ExchangeRateController::_WriteExchangeRate() {
	fstream* stream = _OpenStream();
	if (stream != nullptr) {
		// check if model header exists
		if (this->header._Loaded() == false)
			this->_WriteNewModelHeader(stream, header);

		// write exchange rate info (ExchangeRate model) - should have only one
		char* buffer = excRate->_Serialize();
		_WriteModel(stream, header, buffer);

		// write currencies (rates) into database
		for (size_t i = 0; i < currencies->size(); i++) {
			buffer = currencies->at(i)._Serialize();
			_WriteModel(stream, header, buffer);
		}
		stream->close();
	}
	delete stream;
}

//	Returns currency with the given id
Currency* ExchangeRateController::_GetCurrency(int id) {
	for (size_t i = 0; i < currencies->size(); i++) {
		if (currencies->at(i)._ID() == id)
			return &currencies->at(i);
	}
}