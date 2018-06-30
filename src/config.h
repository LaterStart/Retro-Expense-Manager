#pragma once
#include "utility.h"

extern const char* const title;
extern const char* const version;
extern const char* const database; 
extern const char* const modulesDirectory;

extern char* currentDate;

extern unsigned short width;
extern unsigned short height;

extern const unsigned char horizontalLine;
extern const unsigned char verticalLine;
extern const unsigned char spaceKey;
extern const unsigned char headerSymbol;

const enum class ModelName {
	none,
	mainHeader,
	modelHeader,
	profile,
	account,
	transaction
};

const enum class Field {
	none,
	username,
	pwStatus,
	password,
	currency,
	transactionType,
	account,
	category,
	amount,
	date,
	description
};

const enum class Range {
	none,
	all
};

extern const char* transactionType[];
extern const int transactionType_num;