#pragma once
#include "OComponent.h"
#include "../Controllers/_Controller.h"

const enum class InputType { 
	none, 
	menuSelect,
	text,
	YN,
	password,
	scrollDown
};

class Input : public IOComponent {};

class UserInput : public Input{
private:
	short min_x = 0;
	short max_x = 0;
	struct List {
		char value;
		List* nextNode = nullptr;
		List* previousNode = nullptr;
	};

	List* initial = nullptr;
	List* first = nullptr;
	List* node = nullptr;
	InputType type = InputType::none;	
	Frame* parentFrame = nullptr;
	Cursor pos;
	Display dsp;
	char* buffer = nullptr;

	void _Initialize();
	void _Conclude();
	void _ContinueAccept(char& ch);
	void _ContinueInsert(char& ch);
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
	UserInput(InputType type);
	~UserInput();

	void _SetParentFrame(Frame* parentFrame);
	void _SetType(InputType type);
	void _ReadUserInput();
	void _ClearInput();
	void _HideInput();
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
	bool mandatory = true;
	bool activated = false;
	bool filled = false;
	bool dataField = true;

	virtual bool _InputControl();		
	void _SwitchField(int control);
	
public:		
	FormField(const char* text, InputType type, Field field = Field::none) : Label(text), type(type), dataType(field){}
	FormField(const FormField& copy);
	
	UserInput* inputField = nullptr;
	Field dataType = Field::none;

	void _SetFilledStatus(bool status);
	void _SetActiveStatus(bool status);
	bool _GetActiveStatus() const;
	bool _GetDataStatus() const;
	void _SetParentForm(Form* parentForm);

	virtual void _CreateInputFrame();
	virtual void _ShiftInputFrame(int num);
	virtual FormField* _GetNextField();
	virtual FormField* _GetNextField(FormField* currentField);
	virtual FormField* _GetLastSubField();
	virtual FormField* _GetPreviousField(FormField* currentField);
	virtual FormField* _Store();

	void _Show() override;
	void _Hide() override;
	void _Clear();

	virtual ~FormField();
};

inline void FormField::_SetParentForm(Form* parentForm) {
	this->parentForm = parentForm;
}

inline FormField* FormField::_GetNextField() {
	return nullptr;
}

inline FormField* FormField::_GetNextField(FormField* currentField) {
	return nullptr;
}

inline FormField* FormField::_GetLastSubField() {
	return nullptr;
}

inline FormField* FormField::_GetPreviousField(FormField* currentField) {
	return nullptr;
}

inline bool FormField::_GetDataStatus() const {
	return dataField;
}

inline bool FormField::_GetActiveStatus() const {
	return activated;
}

inline void FormField::_SetActiveStatus(bool status){
	this->activated = status;
}

inline void FormField::_SetFilledStatus(bool status) {
	this->filled = status;
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
	template<typename ... TT>
	OptionField(const char* text, Field field,  TT& ... fields) : FormField(text, InputType::YN, field) {
		this->mandatory = false;
		_AddFields(fields ...);
	}

	OptionField(const OptionField& copy);

	FormField* _GetField(int pos) const;
	void _Show() override;
	void _CreateInputFrame() override;
	void _ShiftInputFrame(int num) override;
	FormField* _GetNextField() override;
	FormField* _GetNextField(FormField* currentField) override;
	FormField* _GetLastSubField() override;
	FormField* _GetPreviousField(FormField* currentField) override;
	FormField* _Store() override;

	~OptionField() override;
};

inline FormField* OptionField::_GetField(int pos) const {
	return optionalFields[pos];
}

class ProfileController;
class UsernameField : public FormField {
private:
	ProfileController& controller;

public:
	UsernameField(const char* text, ProfileController& controller) : FormField(text, InputType::text, Field::username), controller(controller){}
	UsernameField(const UsernameField& copy);

	FormField* _Store() override;
	void _Show() override;
};

class PasswordField : public FormField {
	bool master = false;
	PasswordField* keyField = nullptr;

	void _SetKeyField(PasswordField* keyField);
	bool _VerifyPassword();

public:
	PasswordField(const char* text, bool master = false) : FormField(text, InputType::password, Field::password), master(master){}
	PasswordField(const PasswordField& copy);

	FormField* _Store() override;
	void _Show() override;
};

inline void PasswordField::_SetKeyField(PasswordField* keyField) {
	this->keyField = keyField;
}

class ConfirmField : public FormField {
public: 
	ConfirmField(const char* text) : FormField(text, InputType::YN, Field::none) { 
		this->mandatory = false; 
		this->dataField = false; 
	}
	ConfirmField(const ConfirmField& copy);

	FormField* _Store() override;
	void _Show() override;
};

class Form : public Input, public FrameElement {
	bool status = false;
	int activeFields = 0;
	int fieldNum = 0;
	FormField** fields = nullptr;
	Display message;	
	bool paused = false;
	FormField* lastField = nullptr;

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
	
	void _ShowNextField(FormField* currentField);
	void _ShowPreviousField(FormField* currentField);
	void _EnableOptional(int optFieldNum, FormField* currentField);
	void _DisableOptional(int optFieldNum, FormField* currentField);
	void _DisplayMessage(const char* message);
	void _UpdateActiveFields(int change);
	void _ClearMessage();
	void _SetStatus(bool status);
	void _Exit(FormField* currentField);
	bool _Exit();
	void _SwitchToMenu(FormField* currentField);
	bool _GetStatus() const;
	bool _IsPaused()const ;
	void _Show();
	utility::LinkedList<Data*>* _GetData();

	FormField* _GetNextField(FormField* currentField);

	Form() = default;
	~Form();
};

inline void Form::_UpdateActiveFields(int change) {
	this->activeFields+=change;
	int test = 0;
}

inline void Form::_ClearMessage() {
	this->message._WipeContent();
}

inline void Form::_SetStatus(bool status) {
	this->status = status;
}

inline bool Form::_GetStatus() const {
	return this->status;
}

inline bool Form::_IsPaused() const {
	return this->paused;
}

class InputField : public FormField{
private:
	bool initialized = false;
	bool _InputControl() override;	
public:
	InputField(char* text, InputType type) : FormField(text, type) {}
	void _Show() override;
};