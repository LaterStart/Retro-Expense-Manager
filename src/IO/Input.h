#pragma once
#include "OComponent.h"

const enum class InputType { none, menuSelect, text };
const enum class FieldType { none, text, YN, password, scrollDown, confirm};

class Input {};

class UserInput : public Input {
private:
	struct List {
		char value;
		List* nextNode = nullptr;
		List* previousNode = nullptr;
	};

	List* node;
	InputType type = InputType::none;	

	int _UpdateInput(int& control, char& ch);
	int _VerifyInput(char& ch);
	int _Verify_menuSelect(char& ch);
	int _Verify_text(char& ch);
	
public:
	int length = 0;
	int selection;
	int control;
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

class FormField : public Input, public Label {
	FieldType type = FieldType::none;

public:
	FormField(const char* text, FieldType type) : Label(text), type(type){}
};

class OptionField : public FormField {
	FormField** optionalFields = nullptr;
	int optFieldNum = 0;
	bool condition = false;

	void _AddField(FormField& field);

	template <typename T>
	void _AddFields(T& field) {
		_AddField(field);
	}
	template<typename T, typename ... TT>
	void _AddFields(T& field, TT& ... nextFields) {
		_AddField(field);
		_AddFields(nextFields...);
	}

public:
	template<typename T, typename ... TT>
	OptionField(const char* text, FieldType type, T& field,  TT& ... fields) : FormField(text, type) {
		_AddField(field);
		_AddFields(fields ...);
	}

	void _ActivateOptional();
	void _Show();
};

class Form : public Input, public FrameElement {
	int fieldNum = 0;
	int filledNum = 0;
	FormField** fields = nullptr;
	
	







	void _Show();
	void _InitializeFields();
	void _AddField(FormField& field);		

public:
	template <typename T>
	void _AddFields(T& field) {
		_AddField(field);
	}
	template<typename T, typename ... TT>
	void _AddFields(T& field, TT& ... nextFields) {
		_AddField(field);
		_AddFields(nextFields...);
	}	

	Form() = default;
	~Form() = default;
};