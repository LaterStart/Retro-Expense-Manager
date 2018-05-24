#pragma once

namespace utility {

	char* _ConvertToChar(unsigned int number);
	char* _ConcatenateChar(const char* myChar ...);
	char* _CopyChar(char* char_in);
	char* _CopyChar(const char* char_in);
	char* _GetCurrentDate();

	int _CharLength(char* arr);
	int _CharLength(const char* arr);
	int _CharSize(char* arr);
	
	template <typename T>
	void _RemoveElement(T* &ptr, int pos, int arrSize) {
		T* newArr = new T[arrSize - 1]; int z = 0;
		for (int i = 0; i < arrSize; i++) {
			if (i != pos)
				newArr[z++] = ptr[i];
		}
		delete[]ptr;
		ptr = newArr;
	}
}
