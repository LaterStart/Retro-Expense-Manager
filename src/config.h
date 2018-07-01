#pragma once
#include "utility.h"
#include <vector>
#include <tuple>

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