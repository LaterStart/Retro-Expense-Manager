#include <conio.h>
#include <iostream>
#include "Input.h"
using namespace std;

UserInput::UserInput(InputType type) : type(type) {
	this->componentType = ComponentType::userInput;
}

UserInput::~UserInput(){	
	_ClearInput();
}

void UserInput::_Initialize() {
	if (node == nullptr) {
		node = new List;
		initial = node;
	}

	if (parentFrame != nullptr && type!= InputType::scrollDown && type!= InputType::select) {
		if (type == InputType::date) {
			for (int i = 0; i < 10; i++) {
				if (input[i] == ::spaceKey)
					length--;
			}
		}
		Frame::Coordinates coord = parentFrame->_GetCoordinates();
		if (dsp._IsEmpty() && length > 0) {
			pos._SetXY(coord.x1,coord.y1);
			pos._SetCursorPosition();
			dsp._Display(input);
		}

		int varLngth = (type == InputType::date) ? 10 : length;
		pos._SetXY(coord.x1 + varLngth, coord.y1);
		pos._SetCursorPosition();
		min_x = coord.x1;
		max_x = coord.x2;
	}
	else pos._GetCursorPosition();

	control = ControlKey::none;
	controlKey = false;
	first = node;

	delete[]input;
	input = nullptr;
	if (length > 0) {
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
	delete[]buffer;
	buffer = nullptr;
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

// 
int UserInput::_VerifyInput(char& ch) {
	// esc key
	if (ch == 27) {
		ch = _getch();
		this->control = ControlKey::esc;
		controlKey = true;
		return -1;
	}
	// backspace
	else if (ch == 8) {
		ch = _getch();
		return 7;
	}
	// special keys
	else if (ch == -32) {
		ch = _getch();
		switch (ch) {
			// up arrow
		case 72:
			this->control = ControlKey::upArrow;
			controlKey = true;
			return 6;
			// down arrow
		case 80:
			this->control = ControlKey::downArrow;
			controlKey = true;
			return 6;
		case 75:
			// left arrow
			if (type == InputType::scrollDown) {
				this->control = ControlKey::leftArrow;
				controlKey = true;
				return 6;
			}
			return 8;
		case 77:
			// right arrow
			if (type == InputType::scrollDown) {
				this->control = ControlKey::rightArrow;
				controlKey = true;
				return 6;
			}
			return 9;
		case 73:
			// page up
			return 12;
		case 81:
			// page down
			return 13;
		case 83: 
			// delete key
			return 14;
		default:
			return 0;
		}
	}
	// extended keys
	else if (ch == 0) {		
		ch = _getch();
		switch (ch) {
		case 59:
			// F1 key
			return 10;
		case 60:
			// F2 key
			return 11;
		default: 
			return 0;
		}
	}
	else {		
		_getch();
		switch (type) {
		case InputType::select:
			return _Verify_select(ch);
		case InputType::text:
			return _Verify_text(ch);
		case InputType::YN:
			return _Verify_YN(ch);
		case InputType::password:
			return _Verify_password(ch);
		case InputType::scrollDown:
			return _Verify_scrollDown(ch);
		case InputType::value:
			return _Verify_value(ch);
		case InputType::date:
			return _Verify_date(ch);
		default:
			return 0;
		}
	}
}

int UserInput::_Verify_select(char& ch) {
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

int UserInput::_Verify_scrollDown(char& ch) {
	if (ch == 13)
		return 3;
	else if (ch < 32 || ch > 126)
		return 0;
	else return 15;
}

int UserInput::_Verify_value(char& ch) {
	if (ch == 13)
		return 3;
	else if (length == 10 && ch == 46)
		return 2;
	else if ((length == 11 || length == 12) && (ch >= 48 && ch <= 57)) 
		return 2;
	else if (length >= 10)
		return 0;
	else if (ch == 46) {
		List* checker = node->previousNode;
		int digitCount = 0;
		while (checker != nullptr) {
			if (checker->value == 46)
				return 0;
			else digitCount++;
			checker = checker->previousNode;
		}
		if(digitCount > 0)
			return 2;
		else return 0;
	}
	else if (ch < 48 || ch > 57)		
		return 0;
	else {
		List* checker = node->previousNode;
		while (checker != nullptr) {
			if (checker->value == 46) {
				int decimalCount = 0;
				while (checker != node) {
					checker = checker->nextNode;
					if (checker->value >= 48 && checker->value <= 57)
						decimalCount++;
				}
				if (decimalCount == 2)
					return 0;
				else return 2;
			}
			checker = checker->previousNode;
		}
		return 2;
	}
}

int UserInput::_Verify_date(char& ch) {
	if (ch == 13) {
		bool verify = false;
		if (length == 10) {
			char* temp = new char[length + 1];			
			unsigned int z = 0;
			List* tmp = initial;

			while (tmp != nullptr) {
				temp[z++] = tmp->value;
				tmp = tmp->nextNode;
			}
			temp[length] = '\0';

			verify = utility::_VerifyDate(temp);
			delete[]temp;
		}
		if (verify)
			return 3;
		else return 0;
	}
	if (node->value == '/' && ch != '/')
		return 0;
	else if (node->value == '/' && ch == '/')
		return 16;
	else if (ch < 48 || ch > 57)
		return 0;
	else if (length == 10)
		return 0;
	else return 16;
}

int UserInput::_UpdateInput(int& control, char& ch) {	
	switch (control) {
	case -1:
		// special keys - delete last node and end input
	del: {
		while (node->nextNode != nullptr)
			node = node->nextNode;
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
		// continue - reject input
		return 0;
	case 1:
		// end input - accept input
		return 1;
	case 2:
		// continue - accept input			
		if (node->nextNode == nullptr) {
			_ContinueAccept(ch);
			_DisplayInput(ch);
		}
		// insert character
		else
			_ContinueInsert(ch);
		return 2;
	case 3:	goto del; // enter key pressed
	case 4:
		//	Y/N input - accept input and exit
		_DisplayInput(ch);
		return 1;
	case 5:
		//	password input - hide characters
		//	accept input		
		if (node->nextNode == nullptr) {
			_ContinueAccept(ch);
			ch = '*';
			_DisplayInput(ch);
		}
		else
			// insert character
			_ContinueInsert(ch);
		return 2;
	case 6: 
		// up, down, left, right - arrow
		if (type == InputType::date)
			length = 10;
		if (this->control == ControlKey::upArrow || this->control == ControlKey::downArrow)
			goto del;
		if (type == InputType::date) 
			return 1;
		goto del; 
	case 7:
		//	backspace - delete previous input
		if (node->previousNode != nullptr) {
			if (type == InputType::date) {
				if (node->previousNode->value == '/')
					return 2;
				else {
					dsp._Backspace();	
					length = (node->previousNode->value == ::spaceKey) ? length : length - 1;
					node = node->previousNode;
					node->value = ::spaceKey;
					return 2;
				}
			}				
			if (node->previousNode->previousNode != nullptr) {
				List* deleter = node->previousNode;
				node->previousNode->previousNode->nextNode = node;
				node->previousNode = node->previousNode->previousNode;
				delete deleter;
			}
			else {
				delete node->previousNode;
				node->previousNode = nullptr;
				initial = node;
				first = node;
			}
			length--;
			if (node->nextNode == nullptr)
				dsp._Backspace();
			else {
				Cursor curr;
				_LoadBuffer();
				dsp._WipeContent();
				pos._SetX(min_x);
				dsp._Display(pos, buffer);
				curr._ChangeX(-1);
				curr._SetCursorPosition();
			}
		}
		return 2;
	case 8:
		//	left arrow
		pos._GetCursorPosition();
		if (node->previousNode != nullptr) {
			node = node->previousNode;
			pos._ChangeX(-1);
			pos._SetCursorPosition();
		}
		return 2;
	case 9:
		//	right arrow
		pos._GetCursorPosition();
		if (node->nextNode != nullptr) {
			node = node->nextNode;
			pos._ChangeX(1);
			pos._SetCursorPosition();
		}
		return 2;
	case 10:
		//	F1 key
		this->control = ControlKey::F1;
		controlKey = true;
		goto del;
	case 11:
		//	F2 key
		this->control = ControlKey::F2;
		controlKey = true;
		goto del;
	case 12:
		//	page up
		this->control = ControlKey::pageUp;
		controlKey = true;
		goto del;
	case 13:
		//	page down
		this->control = ControlKey::pageDown;
		controlKey = true;
		goto del;
	case 14:
		//	delete key - delete selected input
		if (type == InputType::date) {
			if (node->value == '/')
				return 2;
			else {
				Cursor tmp;
				dsp._Display(::spaceKey);
				tmp._SetCursorPosition();
				length = (node->value == ::spaceKey) ? length : length - 1;
				node->value = ::spaceKey;
				return 2;
			}
		}
		if (length > 0 && node->nextNode != nullptr) {
			if (node->previousNode != nullptr) {
				node->previousNode->nextNode = node->nextNode;
				if (node->nextNode != nullptr)
					node->nextNode->previousNode = node->previousNode;
			}
			else if (node->nextNode != nullptr)
				node->nextNode->previousNode = nullptr;

			if (node->nextNode != nullptr) {
				List* deleter = node;
				node = node->nextNode;
				delete deleter;
			}
			else if (node->previousNode != nullptr) {
				List* deleter = node;
				initial = node->previousNode;
				delete deleter;
			}
			else initial = node;

			if (node->previousNode == nullptr)
				initial = node;
			first = initial;
			length--;

			Cursor curr;
			_LoadBuffer();
			dsp._WipeContent();
			pos._SetX(min_x);
			dsp._Display(pos, buffer);
			curr._SetCursorPosition();
		}
		return 2;
	case 15:
		// scroll down letter search
		_ContinueAccept(ch);
		goto del;
	case 16:
		// date input
		if (node->nextNode == nullptr)
			return 2;
		else {
			node->value = ch;
			dsp._Display(ch);
			node = node->nextNode;
			if(ch!='/')
				length++;
			return 2;
		}
	default:
		return 0;
	}
}

void UserInput::_ContinueAccept(char& ch) {
	length++;
	node->value = ch;
	node->nextNode = new List;
	node->nextNode->previousNode = node;
	node = node->nextNode;
}

void UserInput::_ContinueInsert(char& ch) {
	length++;
	List* temp = node->previousNode;
	node->previousNode = new List;
	node->previousNode->value = ch;
	node->previousNode->nextNode = node;
	node->previousNode->previousNode = temp;

	if (temp != nullptr) 
		temp->nextNode = node->previousNode;
	else first = node->previousNode;	

	Cursor curr;
	_LoadBuffer();
	dsp._WipeContent();
	pos._SetX(min_x);
	dsp._Display(pos, buffer);
	curr._ChangeX(1);
	curr._SetCursorPosition();
}

void UserInput::_DisplayInput(char& ch) {
	pos._GetCursorPosition();
	if (max_x > 0) {		
		short x = pos._GetX();
		if (x == max_x-1) {
			first = first->nextNode;
			dsp._WipeContent();
			pos._SetX(min_x);
			_LoadBuffer();
			dsp._Display(pos, buffer);
		}
		else dsp._Display(ch);
	}
	else dsp._Display(ch);
}

void UserInput::_LoadBuffer() {
	if(buffer == nullptr)
		buffer = new char[max_x - min_x];

	List* selector = first;
	int z = 0;

	while (selector->nextNode != nullptr) {
		switch (type) {
		case InputType::password:
			buffer[z++] = '*';
			break;
		default:
			buffer[z++] = selector->value;
		}		
		selector = selector->nextNode;
	}
	buffer[z] = '\0';
}

void UserInput::_ClearInput() {
	delete[]input;
	delete[]buffer;
	length = 0;	
	selection = 0;
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
	controlKey = false;
	control = ControlKey::none;	
}

void UserInput::_HideInput() {
	dsp._WipeContent();
}

void UserInput::_SetDefaultInput(char* input) {
	length = utility::_CharLength(input);
	node = new List;
	initial = node;
	for (int i = 0; i < length - 1; i++) {
		node->value = input[i];
		node->nextNode = new List;
		node->nextNode->previousNode = node;
		node = node->nextNode;
	}
	node->value = input[length - 1];
	node = initial;
	this->input = utility::_CopyChar(input);
}

bool InputField::_InputControl() {
	inputField->_ReadUserInput();

	if (inputField->control == ControlKey::upArrow)
		return false;
	else if (inputField->control == ControlKey::downArrow)
		return false;
	else if (inputField->control == ControlKey::esc)
		return false;
	else if (mandatory && inputField->length < 1) 		
		return false;
	else return true;
}

void InputField::_Show() {
	if (!initialized) {
		_CreateInputFrame();
		initialized = true;
	}
	Label::_Show();
	if (_InputControl()) {
		filled = true;
	}
	else {
		if (inputField->control == ControlKey::esc)
			return;
		if (inputField->control == ControlKey::upArrow)
			return;
		if (inputField->control == ControlKey::F1)
			return;
		else this->_Show();
	}
}