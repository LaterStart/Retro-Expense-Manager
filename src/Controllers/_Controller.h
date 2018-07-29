#pragma once
#include <fstream>
#include "../config.h"
#include "../Models/_Header.h"

const enum class Field {
	none, username, pwStatus, password, currency, transactionType, categoryType, account, category, amount, date, description, categoryName, parentCategory,
	accountName, accountType, accountTypeName, multiCurrency
};

const enum class CategoryType {
	mainCategory, subCategory, temporary
};

const enum class TransactionType {
	none = -1, expense, income, transfer, refund
};

const enum class Range {
	none,
	all,
	lastTen,
	thisMonth
};

class UserInput;
struct Data {
	Field field;
	UserInput* input;	

public:
	Data(Field field, UserInput* input) : field(field), input(input) {}
	~Data() = default;

};

class ModelHeader;
class Query {
private:
	bool dateRange = false;
	Range range = Range::none;	
	std::vector<int>* includeIDs = new std::vector<int>;
	std::vector<int>* excludeIDs = new std::vector<int>;

	void _IncludeID(int id);
	void _ExcludeID(int id);

public:
	void _SetRange(Range range);

	template <typename T>
	void _IncludeIDs(T ID) {
		_IncludeID(ID);
	}
	template<typename T, typename ... TT>
	void _IncludeIDs(T ID, TT ... nextIDs) {
		_IncludeID(ID);
		_IncludeIDs(nextIDs...);
	}

	template <typename T>
	void _ExcludeIDs(T ID) {
		_ExcludeID(ID);
	}
	template<typename T, typename ... TT>
	void _ExcludeIDs(T ID, TT ... nextIDs) {
		_ExcludeID(ID);
		_ExcludeIDs(nextIDs...);
	}

	bool _ValidateID(int ID, ModelHeader& header);
	bool _ValidateDate(char* buffer, ModelHeader& header);
	bool _DateRange() const;

	Query() = delete;
	Query(Range range) : range(range){
		switch (range) {
		case Range::thisMonth:
			dateRange = true;
		default:			
			break;
		}
	}
	Query(const Query& copy) {
		this->range = copy.range;
		this->excludeIDs = copy.excludeIDs;
		this->includeIDs = copy.includeIDs;
		this->dateRange = copy.dateRange;
	}
	~Query() {
		delete includeIDs;
		delete excludeIDs;
	}
};

inline void Query::_SetRange(Range range) {
	this->range = range;
}

inline void Query::_IncludeID(int id) {
	includeIDs->push_back(id);
}

inline void Query::_ExcludeID(int id) {
	excludeIDs->push_back(id);
}

inline bool Query::_DateRange() const {
	return this->dateRange;
}

struct Buffer {
	char* data = nullptr;
	int size = 0;
	Buffer* nextNode = nullptr;

	~Buffer(){ delete[]data; }
};

class DataBlock; class Header; class MainHeader; class Model;
class Controller {
private:
	static const int clusterSize = 4096;
	static MainHeader header;	
	static void _LoadHeader();	
	static DataBlock _GetBlock(char* page, int offset, ModelName name);

	void _DeleteBuffers(char* &buffer, char* &dblock, bool containsID = true);
	void _UpdateHeader(std::fstream* stream, Header& header);
	void _UpdateLastNode(std::fstream* stream, Header& header, ModelName name, std::streamoff pageNum);		

protected:		
	static std::fstream* _OpenStream();
	bool _CreateDatabase();
	void _WriteNewModelHeader(std::fstream* stream, ModelHeader& header);
	void _WriteModel(std::fstream* stream, ModelHeader& header, char* buffer);
	void _LoadHeader(ModelHeader& header);	
	char* _GetModel(std::fstream* stream, ModelHeader& header, int ID);
	void _UpdateModel(std::fstream* stream, ModelHeader& header, int ID, char* buffer);
	std::vector<char*>* _GetModels(std::fstream* stream, ModelHeader& header, Query& query);	Controller* ptr;

public:	
	virtual Model* _DeserializeModel(char* buffer) = 0;
};