#pragma once
#include <functional>
#include "OComponent.h"
#include "../Controllers/_Controller.h"
#include "../Models/Category.h"

const enum class InputType { 
	none, 
	select,
	text,
	YN,
	password,
	value,
	scrollDown
};

const enum class ControlKey {
	esc = -1,
	none = 0,
	upArrow = 1,
	downArrow = 2,
	F1 = 3,
	F2 = 4,
	pageUp = 6,
	pageDown = 7
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
	int _Verify_select(char& ch);
	int _Verify_text(char& ch);
	int _Verify_YN(char& ch);
	int _Verify_password(char& ch);
	int _Verify_value(char& ch);
	
public:
	int length = 0;
	int selection = -1;	
	bool check = false;
	char* input = nullptr;
	bool controlKey = false;
	Frame* parentFrame = nullptr;
	ControlKey control = ControlKey::none;

	UserInput() = default;
	UserInput(InputType type);
	~UserInput();

	void _SetParentFrame(Frame* parentFrame);
	void _SetType(InputType type);
	void _ReadUserInput();
	void _ClearInput();
	void _HideInput();
	bool _ControlKey() const;
};

inline void UserInput::_SetType(InputType type) {
	this->type = type;
}

inline void UserInput::_SetParentFrame(Frame* parentFrame) {
	this->parentFrame = parentFrame;
}

inline bool UserInput::_ControlKey() const {
	return this->controlKey;
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
	bool hidden = false;

	virtual bool _InputControl();		
	void _SwitchField(ControlKey control);
	
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
private:
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
	void _Hide() override;
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

class SelectionField : public FormField {
private:
	std::vector<const char*>& options;

public:
	SelectionField(const char* text, std::vector<const char*>& options, Field field) : FormField(text, InputType::select, field), options(options){}

	void _Show() override;
	FormField* _Store() override;
};

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
private:
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
private:
	bool status = false;
	int activeFields = 0;
	int hiddenFields = 0;
	int fieldNum = 0;
	FormField** fields = nullptr;
	Display message;	
	bool paused = false;
	FormField* lastField = nullptr;
	int specialContentHeight = 0;
	
	void _AddField(FormField& field);
	void _InsertField(std::tuple<FormField&, int>);	

	std::function<void(Form&)> event;
	bool eventStatus = false;
	bool eventEnabled = false;

public:
	template <typename T>
	void _AddFields(T& field) {
		_AddField(field);
	}
	template <typename T, typename ... TT>
	void _AddFields(T& field, TT& ... nextFields) {
		_AddField(field);
		_AddFields(nextFields...);
	}		

	template <typename T>
	void _InsertFields(T& field) {
		_InsertField(field);
	}
	template <typename T, typename ... TT>
	void _InsertFields(T& field, TT& ... nextFields) {
		_InsertField(field);
		_InsertFields(nextFields...);
	}
	
	void _InitializeFields();
	void _ShowNextField(FormField* currentField);
	void _ShowPreviousField(FormField* currentField);
	void _EnableOptional(int optFieldNum, FormField* currentField);
	void _DisableOptional(int optFieldNum, FormField* currentField);
	void _DisplayMessage(const char* message);
	void _UpdateActiveFields(int change);
	void _UpdateHiddenFields(int change);
	void _ClearMessage();
	void _SetStatus(bool status);
	void _Exit(FormField* currentField);
	bool _Exit();
	void _SwitchToMenu(FormField* currentField);
	bool _GetStatus() const;
	bool _IsPaused()const ;
	void _Show();
	void _Hide() override;
	void _SetSpecialContentHeight(int height);
	void _AddEvent(std::function<void(Form&)> const& lambda);
	void _SetEventStatus(bool status);
	bool _EventStatus() const;
	bool _EventEnabled() const;	
	void _RemoveFields(int index, int fieldNum);
	utility::LinkedList<Data*>* _GetData();
	Frame::Coordinates _GetSpecialContentCoord();

	FormField* _GetNextField(FormField* currentField);
	FormField* _SelectField(const char* text);

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

inline void Form::_UpdateHiddenFields(int change) {
	this->hiddenFields += change;
}

inline void Form::_SetSpecialContentHeight(int height) {
	this->specialContentHeight = height;
}

inline void Form::_SetEventStatus(bool status) {
	this->eventStatus = status;
}

inline bool Form::_EventStatus() const {
	return this->eventStatus;
}

inline bool Form::_EventEnabled() const {
	return this->eventEnabled;
}

inline void Form::_RemoveFields(int index, int fieldNum) {
	for (int i = 0; i < fieldNum; i++) {
		if (fields[index]->_GetActiveStatus())
			activeFields--;
		fields[index]->_Hide();
		delete fields[index];
		utility::_RemoveElement(fields, index, this->fieldNum);
	}
}

class InputField : public FormField{
private:
	bool initialized = false;
	bool _InputControl() override;	
public:
	InputField(char* text, InputType type) : FormField(text, type) {}
	void _Show() override;
};

template <class element>
class ScrollDown : public FormField {
private:
	std::vector<element>& items;
	int sValue = 0;
	int sFirst = 0;
	int sLast = 5;
	int sMin = 0;
	int sMax = items.size();

public:
	ScrollDown(const char* text, std::vector<element>& items, Field field) : FormField(text, InputType::scrollDown, field), items(items){}

	void _Show() override;
	FormField* _Store() override;
};

template <typename element>
void ScrollDown<element>::_Show() {
	Label::_Show();
	if (!activated) {
		activated = true;
		parentForm->_UpdateActiveFields(1);
	}
	Frame::Coordinates coord = parentForm->_GetSpecialContentCoord();
	Display dsp;
	Cursor pos(coord.x1, coord.y1);
	Cursor sPos(pos);
	pos._ChangeX(1);

	for (int i = sFirst; i < sLast; i++) {
		if (i == sValue) {
			sPos._ChangeY(sValue-sFirst);
			sPos._SetCursorPosition();
			dsp._Display('>');
		}
		dsp._Display(items.at(i), pos);
		pos._ChangeY(1);
	}
	parentForm->_SetSpecialContentHeight(5);

	coord = inputField->parentFrame->_GetCoordinates();
	Cursor iPos(coord.x1, coord.y1);
	inputField->parentFrame->dsp->_WipeContent();
	inputField->parentFrame->dsp->_Display(items.at(sValue), iPos);

	if (_InputControl()) {
		if (inputField->control == ControlKey::pageUp) {
			sValue = (sValue > sMin) ? sValue - 1 : sMin;
			sFirst = (sValue - sFirst < 2) ? sFirst - 1 : sFirst;
			sFirst = (sFirst < sMin) ? sMin : sFirst;
			sLast = (sLast - sFirst > 5) ? sLast - 1 : sLast;
			sLast = (sLast > sMax) ? sLast - 1 : sLast;

			parentForm->_SetSpecialContentHeight(0);
			dsp._WipeContent();
			this->_Show();
		}
		else if (inputField->control == ControlKey::pageDown) {
			sValue = (sValue < sMax-1) ? sValue + 1 : sMax-1;
			sFirst = (sValue - sFirst > 2) ? sFirst + 1 : sFirst;
			sFirst = (sMax - sFirst < 5) ? sFirst - 1 : sFirst;
			sLast  = (sLast-sFirst < 5) ? sLast + 1 : sLast;
			sLast = (sLast > sMax) ? sLast - 1 : sLast;
			
			parentForm->_SetSpecialContentHeight(0);
			dsp._WipeContent();
			this->_Show();
		}
	}
	else {
		dsp._WipeContent();
		_SwitchField(inputField->control);
	}
}

template <typename element>
FormField* ScrollDown<element>::_Store() {
	return new ScrollDown<element>(*this);
}