#include <conio.h>
#include <iostream>
#include "Input.h"
using namespace std;

void UserInput::_ReadUserInput() {
	node = new List;
	char ch; int control;
	while (true) {
		ch = _getch();
		control = _VerifyInput(ch);
		control = _UpdateInput(control, ch);
		if (control == 1)
			break;
	}

	input = new char[length + 1];
	input[length] = '\0';
	unsigned int z = length-1;
	List* deleter;

	while (node != nullptr) {
		input[z--] = node->value;
		deleter = node;
		node = node->previousNode;
		delete deleter;
	}
	delete node;
}

int UserInput::_VerifyInput(char& ch) {
	if (ch == 27) {
		control = -1;
		return 1;
	}

	else {
		switch (type) {
		case InputType::menuSelect:
			return _Verify_menuSelect(ch);
		case InputType::text:
			return _Verify_text(ch);
		default:
			return 0;
		}
	}
}

int UserInput::_Verify_menuSelect(char& ch) {
	if (ch < 48 || ch > 58)
		return 0;
	else {		
		selection = ch - '0';
		return 1;
	}
}

int UserInput::_Verify_text(char& ch) {
	if(ch < 32 || ch > 126)
		return 0;

	else return 2;
}

int UserInput::_UpdateInput(int& control, char& ch) {	
	switch (control) {
	case 0:
		return 0;
	case 1:
		length++;
		node->value = ch;
		return 1;
	case 2:
		length++;
		node->value = ch;
		node->nextNode = new List;
		node->nextNode->previousNode = node;
		node = node->nextNode;
		cout << ch;
		return 2;
	default: 

		return 0;
	}
}

void UserInput::_ClearInput() {
	delete[]input;
	length = 0;
}