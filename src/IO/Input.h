#pragma once
#include <functional>
#include "IOComponent.h"
#include "../Controllers/_Controller.h"

const enum class InputType { 
	none, 
	select,
	text,
	YN,
	password,
	value,
	scrollDown,
	date
};

const enum class ControlKey {
	esc = -1,
	none = 0,
	upArrow = 1,
	downArrow = 2,
	leftArrow = 3,
	rightArrow = 4,
	F1 = 5,
	F2 = 6,
	pageUp = 7,
	pageDown = 8
};

class Input : public IOComponent {};

class UserInput : public Input {
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
	int _Verify_scrollDown(char& ch);
	int _Verify_value(char& ch);
	int _Verify_date(char& ch);
	
public:
	int length = 0;
	int selection = -1;	
	bool check = false;
	char* input = nullptr;
	bool controlKey = false;
	Frame* parentFrame = nullptr;
	ControlKey control = ControlKey::none;

	UserInput() {
		this->componentType = ComponentType::userInput;
	}
	UserInput(InputType type);
	~UserInput();

	void _SetParentFrame(Frame* parentFrame);
	void _SetType(InputType type);
	void _ReadUserInput();
	void _ClearInput();
	void _HideInput();
	void _SetDefaultInput(char* input);
	bool _ControlKey() const;
	InputType _Type() const;
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

inline InputType UserInput::_Type() const {
	return this->type;
}

class Form;
class FormField : public Input, public Label {	
protected:
	InputType type = InputType::none;	
	Form* parentForm = nullptr;	
	Display dsp;
	bool mandatory = true;
	bool activated = false;
	bool filled = false;
	bool dataField = true;
	bool hidden = false;

	virtual bool _InputControl();		
	void _SwitchField(ControlKey control);
	
public:		
	FormField(const char* text, InputType type, Field field = Field::none) : Label(text), type(type), field(field){
		this->IOComponent::componentType = ComponentType::formField;
	}
	FormField(const FormField& copy);
	
	UserInput* inputField = nullptr;
	Field field = Field::none;

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
		this->OComponent::componentType = ComponentType::optionField;
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
	SelectionField(const char* text, std::vector<const char*>& options, Field field) : FormField(text, InputType::select, field), options(options){
		this->OComponent::componentType = ComponentType::selectionField;
	}

	void _Show() override;
	FormField* _Store() override;
};

class ProfileController;
class UsernameField : public FormField {
private:
	ProfileController& controller;

public:
	UsernameField(const char* text, ProfileController& controller) : FormField(text, InputType::text, Field::username), controller(controller){
		this->OComponent::componentType = ComponentType::usernameField;
	}
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
	PasswordField(const char* text, bool master = false) : FormField(text, InputType::password, Field::password), master(master){
		this->OComponent::componentType = ComponentType::passwordField;
	}
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
		this->OComponent::componentType = ComponentType::confirmField;
	}
	ConfirmField(const ConfirmField& copy);

	FormField* _Store() override;
	void _Show() override;
};

class DateField : public FormField {
private:
	bool currentDateDisplayed = false;
public:
	DateField(const char* text, Field field) : FormField(text, InputType::date, field) {
		this->OComponent::componentType = ComponentType::dateField;
	}
	DateField(const DateField& copy);

	FormField* _Store() override;
	void _Show() override;
};

class ModuleManagement;
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
	unsigned short initialYpos = 0;
	unsigned short contentSpace = 1;

	bool exit = false;
	
	void _AddField(FormField& field);
	void _InsertField(std::tuple<FormField&, int>);		

	std::function<void(Form&, FormField*)>* events = nullptr;
	bool* eventStatus = nullptr;	
	bool eventEnabled = false;
	int eventNum = 0;

	ModuleManagement* moduler = nullptr;

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
	bool _SwitchToExtension(const char* moduleName);
	bool _Status() const;
	bool _IsPaused()const ;
	void _Show();
	void _Hide() override;
	void _SetSpecialContentHeight(int height);	
	void _SetEventStatus(int eventID, bool status);
	bool _EventStatus(int eventID) const;
	bool _EventEnabled() const;	
	void _RemoveFields(int index, int fieldNum);
	void _AddEvent(std::function<void(Form&, FormField*)> const& lambda);
	void _RunEvents(FormField* currentField);
	void _LinkModuler(ModuleManagement* moduler);
	void _SetYpos(unsigned short y) override;
	void _ChangeNextYpos(short y);
	void _Break();
	FrameElement* _Clone() override;
	utility::LinkedList<Data*>* _GetData();
	UserInput* _GetData(Field field);
	Frame::Coordinates _GetSpecialContentCoord();

	FormField* _GetNextField(FormField* currentField);
	FormField* _SelectField(Field field);
	FormField* _LastField();
	std::vector<FormField*> _SelectFields(std::vector<Field> fields);

	Form() {
		this->OComponent::componentType = ComponentType::form;
	}
	Form(const Form& copy);
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

inline bool Form::_Status() const {
	return this->status;
}

inline bool Form::_IsPaused() const {
	return this->paused;
}

inline void Form::_UpdateHiddenFields(int change) {
	this->hiddenFields += change;
	this->hiddenFields = (hiddenFields < 0) ? 0 : hiddenFields;
}

inline void Form::_SetSpecialContentHeight(int height) {
	this->specialContentHeight = height;
}

inline void Form::_SetEventStatus(int eventID, bool status) {
	this->eventStatus[eventID] = status;
}

inline bool Form::_EventStatus(int eventID) const {
	return this->eventStatus[eventID];
}

inline bool Form::_EventEnabled() const {
	return this->eventEnabled;
}

inline void Form::_LinkModuler(ModuleManagement* moduler) {
	this->moduler = moduler;
}

inline void Form::_SetYpos(unsigned short y) {
	this->initialYpos = y;
}

inline void Form::_Break() {
	this->exit = true;
}

inline void Form::_ChangeNextYpos(short y) {
	this->Ypos += y;
}

inline FormField* Form::_LastField() {
	return this->lastField;
}

inline FrameElement* Form::_Clone() {
	Form* clone = new Form(*this);
	clone->original = this;
	this->clone = clone;
	return clone;
}

class InputField : public FormField{
private:
	bool initialized = false;
	bool _InputControl() override;	
public:
	InputField(char* text, InputType type) : FormField(text, type) {
		this->OComponent::componentType = ComponentType::inputField;
	}
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
	ScrollDown(const char* text, std::vector<element>& items, Field field) : FormField(text, InputType::scrollDown, field), items(items){		
		sLast = (sLast > sMax) ? sMax : sLast;
		this->OComponent::componentType = ComponentType::scrollDown;
	}
	ScrollDown(const ScrollDown& copy) : FormField(copy.text, copy.type, copy.field), items(copy.items), sMax(copy.items.size()) {
		sValue = copy.sValue;
		sFirst = copy.sFirst;
		sLast = copy.sLast;
		sMin = copy.sMin;
		sLast = (sLast > sMax) ? sMax : sLast;
		this->OComponent::componentType = ComponentType::scrollDown;
	}
	ScrollDown() = delete;

	void _Show() override;
	FormField* _Store() override;
	std::vector<element>& _Items() const;
	void _UpdateScrollDown(bool selectLastItem = true);
	void _InsertItem(element& newItem, int index = 0);
};

template <typename element>
void ScrollDown<element>::_Show() {
	Label::_Show();
	if(items.size() == 0)
		parentForm->_ShowNextField(this);

	if (!activated) {
		activated = true;
		parentForm->_UpdateActiveFields(1);
	}
	else if (hidden) {
		hidden = false;
		parentForm->_UpdateHiddenFields(-1);
	}

	Frame::Coordinates coord = parentForm->_GetSpecialContentCoord();
	Display dsp(this->parentFrame);
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
	int height = (items.size() < 5) ? items.size() : 5;
	parentForm->_SetSpecialContentHeight(height);

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
			sFirst = (sFirst < sMin) ? sMin : sFirst;
			sLast  = (sLast-sFirst < 5) ? sLast + 1 : sLast;
			sLast = (sLast > sMax) ? sMax : sLast;
			
			parentForm->_SetSpecialContentHeight(0);
			dsp._WipeContent();
			this->_Show();
		}
		else {
			if (inputField->length > 0) {
				bool found = false;
				char d = inputField->input[0];
				inputField->_ClearInput();
				for (int i = sValue+1; i < sMax; i++) {
					char a = items.at(i)._Name()[0] + 32;
					char b = items.at(i)._Name()[0] - 32;
					char c = items.at(i)._Name()[0];					
					if (a == d || b == d || c == d) {
						sValue = i;
						sLast = i + 3;
						sLast = (sLast < 5) ? 5 : sLast;
						sLast = (sLast > sMax) ? sMax : sLast;
						sFirst = sLast - 5;
						sFirst = (sFirst < sMin) ? sMin : sFirst;
						found = true;
						break;
					}
				}
				if (found == false) {
					for (int i = sMin; i < sValue; i++) {
						char a = items.at(i)._Name()[0] + 32;
						char b = items.at(i)._Name()[0] - 32;
						char c = items.at(i)._Name()[0];
						if (a == d || b == d || c == d) {
							sValue = i;
							sLast = i + 3;
							sLast = (sLast < 5) ? 5 : sLast;
							sLast = (sLast > sMax) ? sMax : sLast;
							sFirst = sLast - 5;
							sFirst = (sFirst < sMin) ? sMin : sFirst;		
							break;
						}
					}
				}
				parentForm->_SetSpecialContentHeight(0);
				dsp._WipeContent();
				this->_Show();
			}
			else {
				if (inputField->control != ControlKey::esc)
					dsp._WipeContent();
				inputField->selection = sValue;
				this->filled = true;
				parentForm->_SetSpecialContentHeight(0);
				parentForm->_ShowNextField(this);
			}
		}
	}
	else {
		if (inputField->control != ControlKey::esc)
			dsp._WipeContent();
		_SwitchField(inputField->control);
	}
}

template <typename element>
FormField* ScrollDown<element>::_Store() {
	return new ScrollDown<element>(*this);
}

template <typename element>
std::vector<element>& ScrollDown<element>::_Items() const {
	return this->items;
}

template <typename element>
void ScrollDown<element>::_UpdateScrollDown(bool selectLastItem) {
	sMax = items.size();
	sLast = (sValue + 5 > sMax) ? sMax : sValue + 5;
	if (selectLastItem) {
		sValue = sMax - 1;
		sFirst = (sValue - 4 > 0) ? sValue - 4 : 0;
		sLast = sMax;

		coord = inputField->parentFrame->_GetCoordinates();
		Cursor iPos(coord.x1, coord.y1);
		inputField->parentFrame->dsp->_WipeContent();
		inputField->parentFrame->dsp->_Display(items.at(sValue), iPos);
	}
}

template <typename element>
void ScrollDown<element>::_InsertItem(element& newItem, int index) {
	items.insert(items.begin() + index, newItem);
	this->_UpdateScrollDown(false);
}

template <class element>
class ScrollDown_2D : public FormField {
private:
	std::vector<std::vector<element>>& items;
	int** scrollControl = nullptr;
	int* sCurr = nullptr;
	int* sValue = nullptr;
	int* sFirst = nullptr;
	int* sLast = nullptr;
	int* sMin = nullptr;
	int* sMax = nullptr;
	bool subSelect = false;
	int sHeight = 5;
	size_t initSize = items.size();
	element* value = nullptr;

public:
	ScrollDown_2D(const char* text, std::vector<std::vector<element>>& items, Field field) : FormField(text, InputType::scrollDown, field), items(items) {
		this->OComponent::componentType = ComponentType::scrollDown_2D;

		scrollControl = new int*[items.size()+1];
		scrollControl[items.size()] = new int[5];
		// main scroll down controll
		scrollControl[items.size()][0] = 0;
		scrollControl[items.size()][1] = 0;
		scrollControl[items.size()][2] = 5;
		scrollControl[items.size()][3] = 0;
		scrollControl[items.size()][4] = items.size();

		// sub scroll controll
		for (size_t i = 0; i < items.size(); i++) {
			scrollControl[i] = new int[5];
			scrollControl[i][0] = 1;
			scrollControl[i][1] = 1;
			scrollControl[i][2] = 5;
			scrollControl[i][3] = 1;
			scrollControl[i][4] = items[i].size();
		}

		// set last displayed scroll position
		for (size_t i = 0; i <= items.size(); i++) 
			scrollControl[i][2] = (scrollControl[i][2] > scrollControl[i][4]) ? scrollControl[i][4] : scrollControl[i][2];		
		
		// scroll selector
		sCurr = new int;
		*sCurr = 0;
	}

	void _Show() override;
	FormField* _Store() override;
	std::vector<std::vector<element>>& _Items() const;
	void _InsertItem(std::vector<element>& newItem, int index = 0);
	void _UpdateScrollControl(bool selectLastItem = true);
	void _ToggleSubSelect(bool status, int parentID = 0);
	element* _Value() const;
};

template <typename element>
void ScrollDown_2D<element>::_Show() {
	Label::_Show();
	if (items.size() == 0)
		parentForm->_ShowNextField(this);

	if (!activated) {
		activated = true;
		parentForm->_UpdateActiveFields(1);
		// scroll & subscroll display frames
		parentForm->_ParentFrame()->_Split(55, "vertical", "Scroll", "SubScroll");
	}
	else if (hidden) {
		hidden = false;
		parentForm->_UpdateHiddenFields(-1);
	}

	Frame::Coordinates coord = parentForm->_GetSpecialContentCoord();
	Display dsp(parentForm->_ParentFrame()->_Select("Scroll"));
	Cursor pos(coord.x1, coord.y1);
	Cursor sPos(pos);
	pos._ChangeX(1);

	int maxLength = 0;
	int index = 0;
	for (int i = scrollControl[items.size()][1]+1; i < scrollControl[items.size()][2]; i++) {		
		if (items[i].at(0)._DisplayLength() > maxLength) {
			maxLength = items[i].at(0)._DisplayLength();
			
		}
	}
	Frame::Coordinates coord2 = parentForm->_ParentFrame()->_Select("Scroll")->_GetCoordinates();
	int maxWidth = coord2.x2 - coord2.x1 - parentForm->_ParentFrame()->_LeftPadding() - 1;
	maxLength = (maxLength < maxWidth) ? maxLength : maxWidth;	

	for (int i = scrollControl[items.size()][1]; i < scrollControl[items.size()][2]; i++) {
		if (i == scrollControl[items.size()][0] && !subSelect) {
			sPos._ChangeY(scrollControl[items.size()][0] - scrollControl[items.size()][1]);
			sPos._SetCursorPosition();
			dsp._Display('>');
		}
		dsp._Display(items[i].at(0), pos);
		if (i == scrollControl[items.size()][0]) {
			Cursor subPos;
			Cursor subSpos(subPos);
			int distance;
			int length = items[i].at(0)._DisplayLength();
			length = (length > maxLength) ? maxLength : length;
			if (i == index)
				distance = 0;
			else distance = maxLength - length;
			if (items[i].size() > 1) {
				Cursor chain(subSpos);
				chain._ChangeX(1);
				for (int k = 0; k < distance + 1; k++) {
					chain._SetCursorPosition();
					dsp._Display('-');
					chain._ChangeX(1);
				}
			}
			subPos._ChangeX(distance + 3);		
			subSpos._ChangeX(distance + 2);

			if (items[i].size() > 1) {				
				dsp._SetParentFrame(parentForm->_ParentFrame()->_Select("SubScroll"));
				for (int j = scrollControl[*sCurr][1]; j < scrollControl[*sCurr][2]; j++) {
					if (j == scrollControl[*sCurr][0] && subSelect) {
						subSpos._ChangeY(scrollControl[*sCurr][0] - scrollControl[*sCurr][1]);
						subSpos._SetCursorPosition();
						dsp._Display('>');
					}					
					dsp._Display(items[*sCurr].at(j), subPos);					
					subPos._ChangeY(1);
				}
				dsp._SetParentFrame(parentForm->_ParentFrame()->_Select("Scroll"));
			}
		}
		pos._ChangeY(1);
	}
	int mainHeight = (scrollControl[items.size()][4] > 5) ? 5 : scrollControl[items.size()][4];
	int subHeight = (scrollControl[*sCurr][4] > 4) ? 4 : scrollControl[*sCurr][4];
	subHeight =  (subHeight - mainHeight) + (scrollControl[items.size()][0] - scrollControl[items.size()][1]);	
	subHeight = (subHeight < 0) ? 0 : subHeight;
	parentForm->_SetSpecialContentHeight(mainHeight+subHeight);

	coord = inputField->parentFrame->_GetCoordinates();
	Cursor iPos(coord.x1, coord.y1);
	inputField->parentFrame->dsp->_WipeContent();

	if (subSelect)	
		inputField->parentFrame->dsp->_Display(items[*sCurr].at(scrollControl[scrollControl[items.size()][0]][0]), iPos);	
	else inputField->parentFrame->dsp->_Display(items[scrollControl[items.size()][0]].at(0), iPos);

	if (_InputControl()) {
		if (!subSelect) {
			// main scroll control
			sValue = &scrollControl[items.size()][0];
			sFirst = &scrollControl[items.size()][1];
			sLast = &scrollControl[items.size()][2];
			sMin = &scrollControl[items.size()][3];
			sMax = &scrollControl[items.size()][4];
		}
		else {
			// sub scroll control
			sValue = &scrollControl[*sCurr][0];
			sFirst = &scrollControl[*sCurr][1];
			sLast = &scrollControl[*sCurr][2];
			sMin = &scrollControl[*sCurr][3];
			sMax = &scrollControl[*sCurr][4];
		}

		if (inputField->control == ControlKey::pageUp) {
			*sValue = (*sValue > *sMin) ? *sValue - 1 : *sMin;
			*sFirst = (*sValue - *sFirst < 2) ? *sFirst - 1 : *sFirst;
			*sFirst = (*sFirst < *sMin) ? *sMin : *sFirst;
			*sLast = (*sLast - *sFirst > sHeight) ? *sLast - 1 : *sLast;
			*sLast = (*sLast > *sMax) ? *sMax : *sLast;

			if (!subSelect)
				*sCurr = *sValue;

			parentForm->_SetSpecialContentHeight(0);
			dsp._WipeContent();
			this->_Show();
		}
		else if (inputField->control == ControlKey::pageDown) {
			if (*sMax > 0) {
				*sValue = (*sValue < *sMax - 1) ? *sValue + 1 : *sMax - 1;
				*sFirst = (*sValue - *sFirst > 2) ? *sFirst + 1 : *sFirst;
				*sFirst = (*sMax - *sFirst < sHeight) ? *sFirst - 1 : *sFirst;
				*sFirst = (*sFirst < *sMin) ? *sMin : *sFirst;
				*sLast = (*sLast - *sFirst < sHeight) ? *sLast + 1 : *sLast;
				*sLast = (*sLast > *sMax) ? *sMax : *sLast;
			}

			if(!subSelect)
				*sCurr = *sValue;

			parentForm->_SetSpecialContentHeight(0);
			dsp._WipeContent();
			this->_Show();
		}
		else if (inputField->control == ControlKey::rightArrow) {
			if (scrollControl[*sCurr][4] > 1) {
				subSelect = true;
				sHeight = 4;
			}
			parentForm->_SetSpecialContentHeight(0);
			dsp._WipeContent();
			this->_Show();
		}
		else if (inputField->control == ControlKey::leftArrow) {
			subSelect = false;
			sHeight = 5;
			parentForm->_SetSpecialContentHeight(0);
			dsp._WipeContent();
			this->_Show();
		}
		else {
			if (inputField->length > 0) {
				bool found = false;
				char d = inputField->input[0];
				inputField->_ClearInput();
				char a, b, c;
				for (int i = *sValue + 1; i < *sMax; i++) {				
					if (subSelect) {
						a = items[*sCurr].at(i)._Name()[0] + 32;
						b = items[*sCurr].at(i)._Name()[0] - 32;
						c = items[*sCurr].at(i)._Name()[0];
					}
					else {
						a = items[i].at(0)._Name()[0] + 32;
						b = items[i].at(0)._Name()[0] - 32;
						c = items[i].at(0)._Name()[0];
					}					
					if (a == d || b == d || c == d) {
						int diff = (sHeight == 5) ? 3 : 2;
						*sValue = i;
						*sLast = i + diff;
						*sLast = (*sLast < sHeight) ? sHeight : *sLast;
						*sLast = (*sLast > *sMax) ? *sMax : *sLast;
						*sFirst = *sLast - sHeight;
						*sFirst = (*sFirst < *sMin) ? *sMin : *sFirst;
						found = true;
						break;
					}
				}
				if (found == false) {
					char a, b, c;
					for (int i = *sMin; i < *sValue; i++) {						
						if (subSelect) {
							a = items[*sCurr].at(i)._Name()[0] + 32;
							b = items[*sCurr].at(i)._Name()[0] - 32;
							c = items[*sCurr].at(i)._Name()[0];
						}
						else {
							a = items[i].at(0)._Name()[0] + 32;
							b = items[i].at(0)._Name()[0] - 32;
							c = items[i].at(0)._Name()[0];
						}
						if (a == d || b == d || c == d) {
							int diff = (sHeight == 5) ? 3 : 2;
							*sValue = i;
							*sLast = i + diff;
							*sLast = (*sLast < sHeight) ? sHeight : *sLast;
							*sLast = (*sLast > *sMax) ? *sMax : *sLast;
							*sFirst = *sLast - sHeight;
							*sFirst = (*sFirst < *sMin) ? *sMin : *sFirst;
							break;
						}
					}
				}
				parentForm->_SetSpecialContentHeight(0);
				dsp._WipeContent();
				this->_Show();
			}
			else {
				if (inputField->control != ControlKey::esc) {
					dsp._WipeContent();
					parentForm->_SetSpecialContentHeight(0);
				}
				inputField->selection = *sValue;
				this->filled = true;
				if (subSelect)
					this->value = &items[*sCurr].at(scrollControl[scrollControl[items.size()][0]][0]);
				else this->value = &items[scrollControl[items.size()][0]].at(0);
				parentForm->_ShowNextField(this);
			}
		}
	}
	else {
		if (inputField->control != ControlKey::esc) {
			dsp._WipeContent();
			parentForm->_SetSpecialContentHeight(0);
		}
		_SwitchField(inputField->control);
	}
}

template <typename element>
FormField* ScrollDown_2D<element>::_Store() {
	return new ScrollDown_2D<element>(*this);
}

template <typename element>
std::vector<std::vector<element>>& ScrollDown_2D<element>::_Items() const {
	return this->items;
}

template <typename element>
void ScrollDown_2D<element>::_InsertItem(std::vector<element>& newItem, int index) {
	items.insert(items.begin() + index, newItem);
	this->_UpdateScrollControl(false);
}

template <typename element>
void ScrollDown_2D<element>::_UpdateScrollControl(bool selectLastItem) {
	for (size_t i = 0; i < initSize; i++)
		delete[]scrollControl[i];
	delete[]scrollControl;

	scrollControl = new int*[items.size() + 1];
	scrollControl[items.size()] = new int[5];
	
	scrollControl[items.size()][0] = 0;
	scrollControl[items.size()][1] = 0;
	scrollControl[items.size()][2] = 5;
	scrollControl[items.size()][3] = 0;
	scrollControl[items.size()][4] = items.size();
	
	for (size_t i = 0; i < items.size(); i++) {
		scrollControl[i] = new int[5];
		scrollControl[i][0] = 1;
		scrollControl[i][1] = 1;
		scrollControl[i][2] = 5;
		scrollControl[i][3] = 1;
		scrollControl[i][4] = items[i].size();
	}
	
	for (size_t i = 0; i <= items.size(); i++)
		scrollControl[i][2] = (scrollControl[i][2] > scrollControl[i][4]) ? scrollControl[i][4] : scrollControl[i][2];

	// update scroll down
	if (selectLastItem) {
		*sCurr = items.size() - 1;

		scrollControl[items.size()][0] = *sCurr;
		scrollControl[items.size()][1] = (*sCurr - sHeight+1 > 0) ? *sCurr - sHeight+1 : 0;
		scrollControl[items.size()][2] = scrollControl[items.size()][4];

		coord = inputField->parentFrame->_GetCoordinates();
		Cursor iPos(coord.x1, coord.y1);
		inputField->parentFrame->dsp->_WipeContent();
		inputField->parentFrame->dsp->_Display(items[scrollControl[items.size()][0]].at(0), iPos);
	}
	initSize = items.size();	
}

template <typename element>
void ScrollDown_2D<element>::_ToggleSubSelect(bool status, int parentID) {
	this->subSelect = status;
	if (status) {
		*sCurr = parentID;
		sHeight = 4;
		int size = items[*sCurr].size();
		scrollControl[*sCurr][0] = size -1;
		scrollControl[*sCurr][1] = (size - sHeight < 1) ? 1 : size - sHeight;
		scrollControl[*sCurr][2] = scrollControl[*sCurr][4];

		int* sVal_init = sValue;
		int* sFir_init = sFirst;
		int* sLas_init = sLast;
		int* sMin_init = sMin;
		int* sMax_init = sMax;

		sValue = &scrollControl[items.size()][0];
		sFirst = &scrollControl[items.size()][1];
		sLast  = &scrollControl[items.size()][2];
		sMin   = &scrollControl[items.size()][3];
		sMax   = &scrollControl[items.size()][4];

		int diff = 3;
		*sValue = *sCurr;
		*sLast = *sCurr + diff;
		*sLast = (*sLast < 5) ? 5 : *sLast;
		*sLast = (*sLast > *sMax) ? *sMax : *sLast;
		*sFirst = *sLast - 5;
		*sFirst = (*sFirst < *sMin) ? *sMin : *sFirst;

		coord = inputField->parentFrame->_GetCoordinates();
		Cursor iPos(coord.x1, coord.y1);
		inputField->parentFrame->dsp->_WipeContent();
		inputField->parentFrame->dsp->_Display(items[*sCurr].back(), iPos);

		sValue = sVal_init;
		sFirst = sFir_init;
		sLast  = sLas_init;
		sMin   = sMin_init;
		sMax   = sMax_init;
	}
	else sHeight = 5;
}

template <typename element>
element* ScrollDown_2D<element>::_Value() const {
	return this->value;
}