#include <time.h>
#include <iomanip>
#include "utility.h"
#include "config.h"
#include "IO/IOComponent.h"
#include "Controllers/_Controller.h"
#include "Windows.h"

bool Initialize::consoleInit = false;
bool Initialize::controllerInit = false;

Console* Initialize::_Console() {
	if (!consoleInit) {
		Console& ref = Console::_GetInstance();
		Console* myConsole = &ref;
		myConsole->_Initialize();
		myConsole->_DrawFrame();

		consoleInit = true;
		return myConsole;
	}
	return nullptr;
}

void Initialize::_Controller() {
	Controller instance(controllerInit);
}

Initialize::CtrlLoader::CtrlLoader() {
	Initialize::_Controller();
}

Initialize::CtrlLoader _load;

Date::Date(char* text) {
	short* ptr[] = { &day, &month, &year };
	int n = utility:: _CharLength(text);
	for (int i = 0; i < 2; i++) {
		text[2] = '\0';		
		*ptr[i] = utility::_ConvertToInteger(text);
		text += 3;
	}
	*ptr[2] = utility::_ConvertToInteger(text);
}

namespace utility {

	int _CharLength(char* text) {
		if (text == nullptr)
			return 0;
		int z = 0;
		while (text[z] != '\0') z++;
		return z;
	}

	int _CharLength(const char* text) {
		return _CharLength((char*)text);
	}

	int _CharSize(char* arr) {
		if (arr == nullptr)
			return 0;
		int size = 0;
		while (arr[size] != '\0')size++;
		return size + 1 * sizeof(char);
	}

	int _CharSize(const char* arr) {
		return _CharSize((char*)arr);
	}

	char* _ChopChar(char* arr, char* remove) {
		int originalSize = _CharSize(arr);
		int removeLength = _CharLength(remove);
		int pos = _Find(remove, arr);
		if (pos == -1)
			return nullptr;

		char* choppedChar = new char[originalSize - removeLength];
		int z = 0, x = 0, y = pos + removeLength;
		while (z < originalSize) {
			if(z < pos || z >= y)
				choppedChar[x++] = arr[z++];
			else z++;
		}

		return choppedChar;
	}

	char* _ChopChar(char* arr, char* removeOne, char* removeTwo) {
		char* choppedOnce = _ChopChar(arr, removeOne);
		char* choppedTwice = _ChopChar(choppedOnce, removeTwo);
		delete[]choppedOnce;
		return choppedTwice;
	}

	char* _InsertChar(char* arr, char* insert) {
		int oldSize = _CharSize(arr);
		int insertLength = _CharLength(insert);
		int newSize = oldSize + insertLength;
		char* newChar = new char[newSize];

		int j = 0;
		for (j = 0; j < insertLength; j++) {
			newChar[j] = insert[j];
		}
		for (int i = 0; i < oldSize; i++)
			newChar[j++] = arr[i];

		return newChar;
	}

	bool _CompareChar(char* first, char* second) {
		int size1, size2;
		size1 = _CharSize(first);
		size2 = _CharSize(second);
		if (size1 != size2) return 0;

		while (--size1 >= 0)
			if (first[size1] != second[size1]) return false;
		return true;
	}

	bool _CompareChar(const char* first, const char* second) {
		return _CompareChar((char*)first, (char*)second);
	}

	char*_ConcatenateChar(const char* myChar ...) {
		va_list list;
		int size = 0;
		int num = 0;
		char** ptrs = nullptr;
		char** tmp = nullptr;
		for (va_start(list, myChar); myChar != 0; myChar = va_arg(list, const char *)) {
			num++;
			size += _CharLength(myChar);
			tmp = new char*[num];
			tmp[num - 1] = _CopyChar(myChar);
			for (int i = 0; i < num - 1; i++)
				tmp[i] = ptrs[i];
			delete[]ptrs;
			ptrs = tmp;
		}
		char* result = new char[size + 1];
		result[size] = '\0';
		int z = 0, y = 0;
		for (int i = 0; i < num; i++) {
			while (ptrs[i][z] != '\0')
				result[y++] = ptrs[i][z++];
			delete[]ptrs[i];
			z = 0;
		}
		delete[]ptrs;
		return result;
	}

	char* _ConvertToChar(unsigned int number) {
		int z = 0;
		unsigned int y = number;
		if (y == 0)
			z++;
		while (y > 0) {
			y /= 10;
			z++;
		}
		y = number;
		char* myChar = new char[z + 1];
		myChar[z--] = '\0';
		while (z >= 0) {
			myChar[z--] = (y % 10) + '0';
			y /= 10;
		}
		return myChar;
	}

	float _ConvertToFloat(char* text) {
		bool separatorFound = false;
		int n = _CharLength(text);

		int number = 0;
		int decimal = 0;
		int multiply = pow(10,n-1);

		for (int i = 0; i < n; i++) {
			if ((text[i] == 46 || text[i] == 44)) {
				if (i == 0)
					return 0.0;
				else if (!separatorFound) {
					separatorFound = true;
					multiply = n - i;
				}
				else return 0.0;
			}			
			else if (text[i] < 48 || text[i] > 57)
				return 0.0;

			if (separatorFound)
				decimal += multiply * (text[i] - '0');
			else number += multiply * (text[i] - '0');

			multiply /= 10;
		}
		
		return number + (1.0 / decimal);
	}

	int _ConvertToInteger(char* text) {
		int n = _CharLength(text);
		int multiply = pow(10, n-1);

		int number = 0;

		for (int i = 0; i < n; i++) {
			if (text[i] < 48 || text[i] > 57)
				return 0;
			else {
				number += multiply * (text[i] - '0');
				multiply /= 10;
			}
		}
		return number;
	}

	char* _CopyChar(char* char_in) {
		char* char_out = nullptr;
		if (char_in != nullptr) {
			int i = 0;
			while (char_in[i] != '\0') i++;
			char_out = new char[i + 1];
			i = 0;
			while (char_in[i] != '\0')
				char_out[i] = char_in[i++];
			char_out[i] = '\0';
		}
		return char_out;
	}

	char* _CopyChar(const char* char_in) {
		return _CopyChar((char*)char_in);
	}

	void _Countdown(int seconds) {
		Cursor pos;
		Display dsp;
		int n = _DigitNumber(seconds);
		short sx = pos._GetX();
		for (int i = seconds; i > 0; i--) {
			pos._SetX(sx);
			int x = n - _DigitNumber(i);			
			for (int i = 0; i < x; i++) {				
				dsp._Display(pos, 0);
				pos._ChangeX(1);
			}
			pos._SetX(sx + x);
			dsp._Display(pos, i);
			Sleep(1000);
			dsp._WipeContent();
		}
	}

	char* _GetCurrentDate() {
		time_t rawtime = time(&rawtime);
		struct tm timeinfo;
		localtime_s(&timeinfo, &rawtime);

		delete ::currentDate;
		::currentDate = new char[11];
		::currentDate[0] = 0 + '0';
		::currentDate[2] = '/';
		::currentDate[3] = 0 + '0';
		::currentDate[5] = '/';
		::currentDate[10] = '\0';

		if (timeinfo.tm_mday < 10)
			::currentDate[1] = timeinfo.tm_mday + '0';
		else {
			::currentDate[1] = timeinfo.tm_mday % 10 + '0';
			::currentDate[0] = timeinfo.tm_mday / 10 + '0';
		}
		if (timeinfo.tm_mon < 10)
			::currentDate[4] = timeinfo.tm_mon + 1 + '0';
		else {
			::currentDate[4] = timeinfo.tm_mon + 1 % 10 + '0';
			::currentDate[3] = timeinfo.tm_mon + 1 / 10 + '0';
		}
		int year = timeinfo.tm_year + 1900;
		short z = 9;
		while (year > 0) {
			::currentDate[z--] = year % 10 + '0';
			year /= 10;
		}

		return ::currentDate;
	}	

	int _DigitNumber(int num) {
		if (num == 0)
			return 1;
		int count = 0;
		while (num > 0) {
			num /= 10;
			count++;
		}
		return count;
	}

	int _Find(char* word, char* arr) {
		int pos = 0;
		char ch = word[0];
		int size = _CharSize(word);
		char* checker = new char[size];
		checker[size - 1] = '\0';
		do {
			if (arr[pos] == ch) {
				for (int i = 0; i < size - 1; i++)
					checker[i] = arr[pos + i];
				if (strcmp(checker, word) == 0) {
					delete[]checker;
					return pos;
				}
			}
		} while (arr[pos++] != '\0');
		delete[]checker;
		return -1;
	}

	int _ReadAlign(const char* align) {
		if (_CompareChar(align, "left"))
			return 1;
		else if (_CompareChar(align, "right"))
			return 2;
		else if (_CompareChar(align, "center"))
			return 3;
		return 0;
	}

	int _ReadDirection(const char* direction) {
		if (_CompareChar(direction, "horizontal"))
			return 0;
		else if (_CompareChar(direction, "vertical"))
			return 1;
		return 2;
	}

	void _XOR(char* &password) {
		const char* key = "!=)#(98hjdA";
		int j = 0;
		int size = _CharLength(password);
		for (int i = 0; i < size; i++) {
			password[i] ^= key[j++];
		}
	}
}