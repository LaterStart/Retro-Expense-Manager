#pragma once
#include "OComponent.h"

const enum class InputType { 
	none, 
	menuSelect,
	text,
	YN,
	password,
	scrollDown,
	confirm
};

class Input : public IOComponent {};

class UserInput : public Input{
private:
	short min_x = 0;
	short max_x = 0;
	bool saveState = false;
	struct List {
		char value;
		List* nextNode = nullptr;
		List* previousNode = nullptr;
	};

	List* first;
	List* node;
	InputType type = InputType::none;	
	Frame* parentFrame = nullptr;
	Cursor pos;
	Display dsp;
	char* buffer = nullptr;

	void _DisplayInput(char& ch);
	void _LoadBuffer();

	int _UpdateInput(int& control, char& ch);
	int _VerifyInput(char& ch);
	int _Verify_menuSelect(char& ch);
	int _Verify_text(char& ch);
	int _Verify_YN(char& ch);
	int _Verify_password(char& ch);
	
public:
	int length = 0;
	int selection;
	int control;
	bool check = false;
	char* input = nullptr;

	UserInput() = default;
	UserInput(InputType type) : type(type){}
	~UserInput() {
		delete[]input;
		delete[]buffer;
	}

	void _SetParentFrame(Frame* parentFrame);
	void _SetType(InputType type);
	void _ReadUserInput();
	void _ClearInput();
};

inline void UserInput::_SetType(InputType type) {
	this->type = type;
}

inline void UserInput::_SetParentFrame(Frame* parentFrame) {
	this->parentFrame = parentFrame;
}

class Form;
class FormField : public Input, public Label {	
protected:
	InputType type = InputType::none;
	Form* parentForm = nullptr;	
	bool filled = false;
	bool activated = false;
	
public:		
	FormField(const char* text, InputType type) : Label(text), type(type){}
	
	UserInput* inputField = nullptr;
	
	bool _ActiveStatus() const;
	void _SetParentForm(Form* parentForm);
	void _Show() override;
	virtual void _CreateInputFrame();
	virtual void _ShiftInputFrame(int num);
	virtual FormField* _GetNextField(FormField* currentField);

	virtual ~FormField() = default;
};

inline void FormField::_SetParentForm(Form* parentForm) {
	this->parentForm = parentForm;
}

inline FormField* FormField::_GetNextField(FormField* currentField) {
	return nullptr;
}

inline bool FormField::_ActiveStatus() const {
	return activated;
}

class OptionField : public FormField {
	FormField** optionalFields = nullptr;
	int optFieldNum = 0;
	bool enabled = true;

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
	OptionField(const char* text, T& field,  TT& ... fields) : FormField(text, InputType::YN) {
		_AddField(field);
		_AddFields(fields ...);
	}

	FormField* _GetField(int pos) const;
	void _Show() override;
	void _CreateInputFrame() override;
	void _ShiftInputFrame(int num) override;
	FormField* _GetNextField(FormField* currentField) override;
};

inline FormField* OptionField::_GetField(int pos) const {
	return optionalFields[pos];
}

class PasswordField : public FormField {
	bool master = false;
	PasswordField* keyField = nullptr;

	void _SetKeyField(PasswordField* keyField);
	void _VerifyPassword();

public:
	PasswordField(const char* text) : FormField(text, InputType::password){}
	PasswordField(const char* text, bool master) : FormField(text, InputType::password), master(master){}
	void _Show() override;
};

inline void PasswordField::_SetKeyField(PasswordField* keyField) {
	this->keyField = keyField;
}

class Form : public Input, public FrameElement {
	int activeFields = 0;
	int currField = 0;
	int fieldNum = 0;
	int filledNum = 0;
	FormField** fields = nullptr;
	Display message;
	
	void _Show();
	void _InitializeFields();

	template <typename T>
	void _AddField(T& field) {
		FormField** pp = new FormField*(nullptr);
		utility::_AddElement(fields, *pp, fieldNum);
		fields[fieldNum - 1] = &field;
		delete pp;
	}

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
	
	void _ShowNextField(FormField* currentField);
	void _ShowPreviousField(FormField* currentField);
	void _EnableOptional(int optFieldNum);
	void _DisableOptional(int optFieldNum);
	void _ChangeCurrentField(int change);
	void _DisplayMessage(const char* message);
	void _UpdateActiveFields(int change);
	void _ClearMessage();

	FormField* _GetNextField(FormField* currentField) const;

	Form() = default;
	~Form() = default;
};

inline void Form::_ChangeCurrentField(int change) {
	this->currField += change;
}

inline void Form::_UpdateActiveFields(int change) {
	this->activeFields+=change;
}

inline void Form::_ClearMessage() {
	this->message._ClearContent();
}