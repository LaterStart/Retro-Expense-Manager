#include <conio.h>
#include <iostream>
#include "Input.h"
using namespace std;

UserInput::UserInput(InputType type) : type(type) {}

UserInput::~UserInput(){
	_ClearInput();
}

void UserInput::_Initialize() {
	if (node == nullptr) {
		node = new List;
		first = node;
		initial = node;
	}	

	if (parentFrame != nullptr) {
		Frame::Coordinates coord = parentFrame->_GetCoordinates();
		if (dsp._IsEmpty() && length > 0) {
			pos._SetXY(coord.x1,coord.y1);
			pos._SetCursorPosition();
			dsp._Display(input);
		}

		if (length > 0)
			coord.x1 += length;
	
		pos._SetXY(coord.x1, coord.y1);
		pos._SetCursorPosition();
		min_x = coord.x1;
		max_x = coord.x2;
	}
	else pos._GetCursorPosition();

	selection = 0;
	control = 0;
	check = 0;

	if (length > 0) {
		delete[]input;
		delete[]buffer;
		while (node->nextNode != nullptr) {
			node = node->nextNode;
		}
		node->nextNode = new List;
		node->nextNode->previousNode = node;
		node = node->nextNode;
	}
}

void UserInput::_Conclude() {
	if (length > 0) {
		input = new char[length + 1];
		input[length] = '\0';
		unsigned int z = length - 1;

		while (node != nullptr) {
			input[z--] = node->value;
			node = node->previousNode;
		}		
	}
	node = initial;
}

void UserInput::_ReadUserInput() {
	_Initialize();
	char ch; 
	int control;

	while (true) {
		ch = _getch();
		control = _VerifyInput(ch);
		control = _UpdateInput(control, ch);
		if (control == 1)
			break;
	}

	_Conclude();
}

int UserInput::_VerifyInput(char& ch) {
	if (ch == 27) {
		control = -1;
		return -1;
	}
	else if (ch == -32) {
		ch = _getch();
		switch (ch) {
		case 72:
			control = 1;
			return 6;
		case 80:
			control = 2;
			return 6;
		default:
			return 0;
		}
	}
	else if (ch == 8)
		return 7;

	else {
		switch (type) {
		case InputType::menuSelect:
			return _Verify_menuSelect(ch);
		case InputType::text:
			return _Verify_text(ch);
		case InputType::YN:
			return _Verify_YN(ch);
		case InputType::password:
			return _Verify_password(ch);
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
	if (ch == 13)
		return 3;
	else if (ch < 32 || ch > 126)
		return 0;			
	else return 2;
}

int UserInput::_Verify_YN(char& ch) {
	if (ch == 89 || ch == 89 + 32) {
		check = true;
		return 4;
	}
	else if(ch == 78 || ch == 78 + 32) {
		check = false;
		return 4;
	}
	else return 0;
}

int UserInput::_Verify_password(char& ch) {
	if (ch == 13)
		return 3;
	else if (ch < 32 || ch > 126)
		return 0;
	else return 5;
}

int UserInput::_UpdateInput(int& control, char& ch) {	
	switch (control) {
	case -1: 
	del: {
		if (node->previousNode != nullptr) {
			List* deleter;
			deleter = node;
			node = node->previousNode;
			node->nextNode = nullptr;
			delete deleter;
		}
		
	}
		return 1;
	case 0:
		return 0;
	case 1:
		return 1;
	case 2:
		length++;
		node->value = ch;
		node->nextNode = new List;
		node->nextNode->previousNode = node;
		node = node->nextNode;
		_DisplayInput(ch);
		return 2;
	case 3:	goto del;
	case 4:
		_DisplayInput(ch);
		return 1;
	case 5:
		length++;
		node->value = ch;
		node->nextNode = new List;
		node->nextNode->previousNode = node;
		node = node->nextNode;
		ch = '*';
		_DisplayInput(ch);
		return 2;
	case 6: goto del;
	case 7: {
		if (node->previousNode != nullptr) {
			List* back;
			back = node;
			node = node->previousNode;
			node->nextNode = nullptr;
			delete back;
			length--;
			dsp._Backspace();
		}
		else if (node->previousNode == nullptr && length>0) {
			length--;
			dsp._Backspace();
		}
	}	
		return 2;
	default: 
		return 0;
	}
}

void UserInput::_DisplayInput(char& ch) {
	pos._GetCursorPosition();
	if (max_x > 0) {		
		short x = pos._GetX();
		if (x == max_x - 2 && buffer == nullptr) {
			buffer = new char[max_x - min_x];
			_LoadBuffer();
			dsp._Display(ch);
		}
		else if (x == max_x-1) {
			first = first->nextNode;
			dsp._WipeContent();			
			pos._SetX(min_x);
			_LoadBuffer();
			dsp._Display(pos,buffer);
		}
		else dsp._Display(ch);
	}
	else dsp._Display(ch);
}

void UserInput::_LoadBuffer() {	
	List* selector = first;
	int z = 0;
	while (selector->nextNode != nullptr) {
		buffer[z++] = selector->value;
		selector = selector->nextNode;
	}
	buffer[z] = '\0';
}

void UserInput::_ClearInput() {
	delete[]input;
	delete[]buffer;
	length = 0;
	input = nullptr;
	buffer = nullptr;
	first = nullptr;
	initial = nullptr;
	dsp._WipeContent();
	List* deleter;
	while (node != nullptr) {
		deleter = node;
		node = node->nextNode;
		delete deleter;
	}	
	node = nullptr;
}

void UserInput::_HideInput() {
	dsp._WipeContent();
}