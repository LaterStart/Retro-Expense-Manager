#pragma once

const enum InputType { none, menuSelect, text };

class Input {};

class UserInput : public Input {
private:
	struct List {
		char value;
		List* nextNode = nullptr;
		List* previousNode = nullptr;
	};

	List* node;
	InputType type = none;	

	int _UpdateInput(int& control, char& ch);
	int _VerifyInput(char& ch);
	int _Verify_menuSelect(char& ch);
	int _Verify_text(char& ch);
	
public:
	int length = 0;
	int selection;
	char* input = nullptr;

	UserInput() = default;
	UserInput(InputType type) : type(type){}
	~UserInput() {
		delete[]input;
	}

	void _SetType(InputType type);
	void _ReadUserInput();
	void _ClearInput();
};

inline void UserInput::_SetType(InputType type) {
	this->type = type;
}