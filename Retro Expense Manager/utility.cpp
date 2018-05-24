#include <stdarg.h> 
#include <time.h>
#include "utility.h"

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

	char* _GetCurrentDate() {
		time_t rawtime = time(&rawtime);
		struct tm timeinfo;
		localtime_s(&timeinfo, &rawtime);

		char* currentDate_char = new char[11];
		currentDate_char[0] = 0 + '0';
		currentDate_char[2] = '/';
		currentDate_char[3] = 0 + '0';
		currentDate_char[5] = '/';
		currentDate_char[10] = '\0';

		if (timeinfo.tm_mday < 10)
			currentDate_char[1] = timeinfo.tm_mday + '0';
		else {
			currentDate_char[1] = timeinfo.tm_mday % 10 + '0';
			currentDate_char[0] = timeinfo.tm_mday / 10 + '0';
		}
		if (timeinfo.tm_mon < 10)
			currentDate_char[4] = timeinfo.tm_mon + 1 + '0';
		else {
			currentDate_char[4] = timeinfo.tm_mon + 1 % 10 + '0';
			currentDate_char[3] = timeinfo.tm_mon + 1 / 10 + '0';
		}
		int year = timeinfo.tm_year + 1900;
		short z = 9;
		while (year > 0) {
			currentDate_char[z--] = year % 10 + '0';
			year /= 10;
		}

		return currentDate_char;
	}	
}