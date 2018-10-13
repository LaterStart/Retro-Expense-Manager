#include "config.h"
using std::vector;

const char* const title = "Retro Expense Manager";
const char* const version = "v2.1";
const char* const database = "database.dat";
const char* const modulesDirectory = "src\\Modules\\";

char* currentDate = nullptr;

unsigned short width = 120;
unsigned short height = 30;

const unsigned char horizontalLine = 196;
const unsigned char verticalLine = 179;
const unsigned char spaceKey = 32;
const unsigned char	headerSymbol = 170;

const bool DEBUG = false;