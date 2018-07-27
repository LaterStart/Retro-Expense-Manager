#include "Input.h"
#include "../Controllers/ProfileController.h"
#include "../Modules/ModuleManagement.h"
#include "../config.h"

void Form::_AddField(FormField& field) {
	FormField** pp = new FormField*(nullptr);
	utility::_AddElement(fields, *pp, fieldNum);
	fields[fieldNum - 1] = field._Store();
	delete pp;
}

void Form::_InsertField(std::tuple<FormField&, int> tuple) {
	FormField** pp = new FormField*(nullptr);	
	utility::_InsertElement(fields, *pp, std::get<1>(tuple), fieldNum);
	fields[std::get<1>(tuple)] = std::get<0>(tuple)._Store();
	delete pp;
}

void OptionField::_AddField(FormField& field) {
	FormField** pp = new FormField*(nullptr);
	utility::_AddElement(optionalFields, *pp, optFieldNum);
	optionalFields[optFieldNum - 1] = field._Store();
	delete pp;
}

void FormField::_CreateInputFrame() {
	Frame::Coordinates frameCoord = parentFrame->_GetCoordinates();
	Frame::Coordinates fieldCoord;
	fieldCoord.x1 = frameCoord.x1 + padding;
	fieldCoord.x1 = frameCoord.x1 + padding + length + 1;
	fieldCoord.x2 = frameCoord.x2 - 1;
	fieldCoord.y1 = frameCoord.y1 + Ypos;
	fieldCoord.y2 = fieldCoord.y1 + 1;
	parentFrame->_CreateSubFrame(text, fieldCoord);
	parentFrame->_Select(text)->_SetDisplay(*new Display(parentFrame->_Select(text)));
	inputField = new UserInput(type);
	inputField->_SetParentFrame(parentFrame->_Select(text));
}

void OptionField::_CreateInputFrame() {
	FormField::_CreateInputFrame();
	for (int i = 0; i < optFieldNum; i++) {
		optionalFields[i]->_SetParentFrame(parentFrame);
		optionalFields[i]->_SetPadding(padding);
		optionalFields[i]->_SetYpos(this->Ypos+i+1);		
		optionalFields[i]->_CreateInputFrame();	
		optionalFields[i]->_SetParentForm(parentForm);
		parentForm->_ChangeNextYpos(1);
	}
}

void FormField::_ShiftInputFrame(int num) {	
	Frame::Coordinates coord = parentFrame->_Select(text)->_GetCoordinates();
	coord.y1 += num;
	coord.y2 += num;
	parentFrame->_Select(text)->_ChangeCoordinates(coord);
	this->_Hide();
}

void OptionField::_ShiftInputFrame(int num) {
	FormField::_ShiftInputFrame(num);
	for (int i = 0; i < optFieldNum; i++)
		optionalFields[i]->_ShiftInputFrame(num);
}

void Form::_InitializeFields() {
	Ypos = initialYpos;
	for (int i = 0; i < fieldNum; i++) {
		int memPos = fields[i]->_Ypos();
		fields[i]->_SetYpos(Ypos++);
		if (fields[i]->inputField == nullptr) {
			fields[i]->_SetParentFrame(parentFrame);
			fields[i]->_SetPadding(padding);
			fields[i]->_SetParentForm(this);
			fields[i]->_CreateInputFrame();			
		}
		else {
			int diff = fields[i]->_Ypos() - memPos;
			if (diff != 0)
				fields[i]->_ShiftInputFrame(diff);
		}
	}
}

bool FormField::_InputControl() {
	if (!activated) {
		parentForm->_UpdateActiveFields(1);
		activated = true;
	}
	else if (hidden) {
		hidden = false;
		parentForm->_UpdateHiddenFields(-1);
	}
	inputField->_ReadUserInput();
	parentForm->_ClearMessage();

	if (inputField->control == ControlKey::upArrow)
		return false;
	else if (inputField->control == ControlKey::downArrow) {
		if (inputField->_Type() == InputType::date) {
			if (!utility::_VerifyDate(inputField->input)) {
				parentForm->_DisplayMessage("Invalid Date.");
				filled = false;
			}
		}
		else if (mandatory && (inputField->selection < 0 && inputField->length < 1)) {
			if (inputField->_Type() != InputType::scrollDown) {
				parentForm->_DisplayMessage("This field cannot be blank.");
				filled = false;
			}
		}
		return false;
	}
	else if (inputField->control == ControlKey::esc)
		return false;
	else if (inputField->control == ControlKey::F1)
		return false;
	else {
		bool condition;
		if (inputField->_ControlKey())
			condition = true;
		else if (mandatory && (inputField->selection < 0 && inputField->length < 1)) {
			if (inputField->_Type() != InputType::scrollDown) {
				parentForm->_DisplayMessage("This field cannot be blank.");
				return false;
			}			
			else condition = true;
		}		
		return true;
	}
}

void FormField::_SwitchField(ControlKey control) {
	if(control != ControlKey::esc && control != ControlKey::F1)
		parentForm->_SetSpecialContentHeight(0);
	switch (control) {
	case ControlKey::esc:
		parentForm->_Exit(this);
		break;
	case ControlKey::upArrow:
		parentForm->_ShowPreviousField(this);
		break;
	case ControlKey::downArrow:
		if(filled)
			parentForm->_ShowNextField(this);
		else this->_Show();
		break;
	case ControlKey::F1:
		parentForm->_SwitchToMenu(this);
		break;
	default:
		this->_Show();
	}
}

void FormField::_Show() {
	Label::_Show();
	if (_InputControl()) {
		filled = true;
		parentForm->_ShowNextField(this);
	}
	else _SwitchField(inputField->control);
}

void UsernameField::_Show() {
	Label::_Show();
	if (_InputControl()) {
		if (controller._Exists(inputField->input)) {
			parentForm->_DisplayMessage("Username already exists.");
			filled = false;
			this->_Show();
		}
		else {
			filled = true;
			parentForm->_ShowNextField(this);
		}		
	}
	else {
		if (inputField->control == ControlKey::downArrow && controller._Exists(inputField->input)) {
			parentForm->_DisplayMessage("Username already exists.");
			this->_Show();
		}
		else {
			parentForm->_ClearMessage();
			_SwitchField(inputField->control);
		}
	}
}

void OptionField::_Show() {
	Label::_Show();
	if (_InputControl()) {
		if (inputField->check == condition) {
			if (enabled == false)
				parentForm->_EnableOptional(optFieldNum, this);
			enabled = true;
		}
		else if (enabled) {			
			for (int i = 0; i < optFieldNum; i++)
				if (optionalFields[i]->_GetActiveStatus() == true) {
					optionalFields[i]->_SetActiveStatus(false);	
					optionalFields[i]->_SetFilledStatus(false);
					optionalFields[i]->_Clear();	
					parentForm->_UpdateActiveFields(-1);
				}
			enabled = false;
			parentForm->_DisableOptional(optFieldNum, this);			
		}
		filled = true;
		parentForm->_ShowNextField(this);
	}
	else _SwitchField(inputField->control);
}

void PasswordField::_Show() {
	Label::_Show();
	bool control = true;

	if (_InputControl()) {
		if (inputField->length < 5) {
			parentForm->_DisplayMessage("Password must be at least 5 characters long.");
			this->_Show();
		}
		else {			
			if (master) {
				PasswordField* slave = dynamic_cast<PasswordField*>(parentForm->_GetNextField(this));
				slave->_SetKeyField(this);
				control = true;
			}
			else if (keyField != nullptr) {
				control = _VerifyPassword();
				dataField = false;
			}

			filled = true;			
			if (control) {
				parentForm->_ClearMessage();
				parentForm->_ShowNextField(this);
			}
			else parentForm->_ShowPreviousField(this);
		}
	}
	else {
		if (inputField->control == ControlKey::downArrow && keyField != nullptr) {
			if (utility::_CompareChar(inputField->input, keyField->inputField->input))
				_SwitchField(inputField->control);
			else {
				parentForm->_DisplayMessage("Passwords didn't match.");
				this->_Show();
			}
		}
		else {
			parentForm->_ClearMessage();
			_SwitchField(inputField->control);
		}
	}
}

bool PasswordField::_VerifyPassword() {
	if (utility::_CompareChar(inputField->input, keyField->inputField->input) == false) {
		keyField->inputField->_ClearInput();
		keyField->filled = false;
		this->inputField->_ClearInput();
		this->filled = false;
		parentForm->_DisplayMessage("Passwords didn't match.");	
		return false;
	}
	else {
		parentForm->_ClearMessage();
		return true;
	}
}

void SelectionField::_Show() {
	Label::_Show();
	if (!activated) {
		activated = true;
		parentForm->_UpdateActiveFields(1);
	}	
	else if (hidden) {
		hidden = false;
		parentForm->_UpdateHiddenFields(-1);
	}

	Frame::Coordinates coord = parentForm->_GetSpecialContentCoord();	
	Cursor pos(coord.x1, coord.y1);
	char num[] = { "[ ] " };
	for (unsigned int i = 0; i < options.size(); i++) {
		num[1] = i + 1 + '0';
		dsp._Display(pos, num, options[i]);
		pos._SetX(coord.x1);
		pos._ChangeY(1);
	}
	parentForm->_SetSpecialContentHeight(options.size());
	coord = inputField->parentFrame->_GetCoordinates();
	if(inputField->selection > 0)
		Cursor iPos(coord.x1 + utility::_CharLength(options[inputField->selection -1]), coord.y1);	
	else Cursor iPos(coord.x1, coord.y1);

	if (_InputControl()) {
		if (inputField->selection > 0 && inputField->selection <= (int)options.size()) {
			inputField->selection--;
			dsp._WipeContent();
			Frame::Coordinates coord = inputField->parentFrame->_GetCoordinates();
			Cursor pos(coord.x1, coord.y1);
			inputField->parentFrame->dsp->_WipeContent();
			inputField->parentFrame->dsp->_Display(pos, options[inputField->selection]);

			filled = true;
			parentForm->_SetSpecialContentHeight(0);
			parentForm->_ShowNextField(this);
		}
		else {
			if (inputField->control != ControlKey::esc)
				dsp._WipeContent();
			_SwitchField(inputField->control);
		}
	}
	else {
		if (inputField->control != ControlKey::esc && inputField->control != ControlKey::F1)
			dsp._WipeContent();

		_SwitchField(inputField->control);
	}
}

void ConfirmField::_Show() {
	Label::_Show();
	if (_InputControl()) {
		this->_Clear();
		activated = false;
		parentForm->_UpdateActiveFields(-1);
		if (inputField->check == true) {		
			parentForm->_Hide();
			parentForm->_DisplayMessage("Saved successfully");
			parentForm->_SetStatus(true);
			Display* dsp = _GetDisplay();		
			dsp->_Loading();
			parentForm->_Break();
		}
	}
	else {
		ControlKey control = inputField->control;
		this->_Clear();
		activated = false;
		parentForm->_UpdateActiveFields(-1);
		_SwitchField(control);
	}
}

void FormField::_Clear() {
	Label::_Hide();
	this->inputField->_ClearInput();	
}

void Form::_DisplayMessage(const char* message) {
	_ClearMessage();
	Frame::Coordinates coord = _GetSpecialContentCoord();	
	Cursor mssgPos(coord.x1, coord.y1);
	this->message._Display(mssgPos, message);
}

void Form::_EnableOptional(int optFieldNum, FormField* currentField) {
	FormField* nextField = _GetNextField(currentField);
	while (nextField != nullptr) {
		if (nextField->_GetActiveStatus()) {
			nextField->_SetActiveStatus(false);
			_UpdateActiveFields(-1);
		}
		nextField->_SetYpos(nextField->_Ypos() + optFieldNum);
		nextField->_ShiftInputFrame(optFieldNum);
		nextField = _GetNextField(nextField);	
	}
}

void Form::_DisableOptional(int optFieldNum, FormField* currentField) {
	FormField* nextField = _GetNextField(currentField);
	while (nextField != nullptr) {	
		if (nextField->_GetActiveStatus()) {
			nextField->_SetActiveStatus(false);
			_UpdateActiveFields(-1);
		}
		nextField->_SetYpos(nextField->_Ypos() - optFieldNum);
		nextField->_ShiftInputFrame(optFieldNum*-1);
		nextField = _GetNextField(nextField);		
	}
}

void Form::_Show() {
	contentSpace = 1;
	if (paused) {
		paused = false;
		_SetSpecialContentHeight(0);
		lastField->_Show();
	}
	else {
		_InitializeFields();
		_ShowNextField(nullptr);		
	}
}

FormField* Form::_GetNextField(FormField* currentField) {
	FormField* field;
	for (int i = 0; i < fieldNum; i++) {
		field = currentField->_GetNextField();
		if (field != nullptr)
			return field;
		field = fields[i]->_GetNextField(currentField);
		if (field != nullptr && field != fields[i])
			return field;
		else if (field == fields[i] && i + 1 < fieldNum)
			return fields[i + 1];
		else if (fields[i] == currentField && i + 1 < fieldNum)
			return fields[i + 1];	
	}
	return nullptr;
}

FormField* OptionField::_GetNextField(FormField* currentField) {
	if (enabled) {
		for (int i = 0; i < optFieldNum; i++) {
			if (optionalFields[i] == currentField && i + 1 < optFieldNum)
				return optionalFields[i + 1];
			else if (optionalFields[i] == currentField && i == optFieldNum - 1)
				return this;
		}
	}
	return nullptr;
}

FormField* OptionField::_GetNextField() {
	if (enabled && optFieldNum > 0)
		return optionalFields[0];
	return nullptr;
}

FormField* OptionField::_GetPreviousField(FormField* currentField) {
	for (int i = 0; i < optFieldNum; i++) {
		if (enabled && optionalFields[i] == currentField && i > 0)
			return optionalFields[i - 1];	
		else if (optionalFields[i] == currentField && i == 0)
			return this;
	}
	return nullptr;
}

FormField* OptionField::_GetLastSubField() {
	if (enabled && optFieldNum > 0)
		return optionalFields[optFieldNum - 1];
	else return nullptr;
}

void Form::_ShowNextField(FormField* currentField) {
	if (eventEnabled)
		_RunEvents(currentField);
	if (exit)
		return;

	if (currentField == nullptr) 
		fields[0]->_Show();
	else {
		FormField* field = currentField->_GetNextField();
		if (field != nullptr) {
			field->_Show();
		}
		else {
			for (int i = 0; i < fieldNum; i++) {
				field = fields[i]->_GetNextField(currentField);
				if (field != nullptr && field != fields[i]) {
					field->_Show();
					break;
				}
				else if (field != nullptr && field == fields[i] && i + 1 < fieldNum) {
					fields[i + 1]->_Show();
					break;
				}
				else if (fields[i] == currentField && i + 1 < fieldNum) {
					fields[i + 1]->_Show();
					break;
				}
			}
		}
	}
}

void Form::_ShowPreviousField(FormField* currentField) {
	if (fields[0] == currentField)
		fields[0]->_Show();
	else {
		FormField* field;
		for (int i = 0; i < fieldNum; i++) {
			field = fields[i]->_GetPreviousField(currentField);
			if (field != nullptr) {
				field->_Show();
				break;
			}
			else if (fields[i] == currentField && i > 0) {
				field = fields[i - 1]->_GetLastSubField();
				if (field != nullptr) {
					fields[i - 1]->_GetLastSubField()->_Show();
					break;
				}
				else {
					fields[i - 1]->_Show();
					break;
				}
			}
		}		
	}
}

void Form::_Exit(FormField* currentField) {
	_DisplayMessage("Are you sure you want to cancel ? Y/N : ");
	UserInput confirm(InputType::YN);
	confirm._ReadUserInput();
	if (confirm.check == true) {
		exit = true;
		status = false;
	}
	else {
		_ClearMessage();
		confirm._ClearInput();
		specialContentHeight = 0;
		currentField->_Show();
	}
}

bool Form::_Exit() {
	_DisplayMessage("Are you sure you want to cancel ? Y/N : ");
	UserInput confirm(InputType::YN);
	confirm._ReadUserInput();
	if (confirm.check == true)
		return true;
	else {
		_ClearMessage();
		confirm._ClearInput();
		return false;
	}
}

void Form::_SwitchToMenu(FormField* currentField) {
	status = false;
	paused = true;
	lastField = currentField;
}

utility::LinkedList<Data*>* Form::_GetData() {	
	FormField* field = fields[0];
	Data* data = new Data(fields[0]->field, fields[0]->inputField);
	utility::LinkedList<Data*>* list = new utility::LinkedList<Data*>(data);
	utility::LinkedList<Data*>* first = list;
	while (field != nullptr) {		
		field = _GetNextField(field);
		if (field != nullptr && field->_GetDataStatus()) {
			data = new Data(field->field, field->inputField);
			list->_AddNextLink(data);
			list = list->nextNode;
		}
	}
	return first;
}

UserInput* Form::_GetData(Field field_) {
	FormField* field = fields[0];
	while (field != nullptr) {
		if (field != nullptr && field->field == field_) {
			return field->inputField;
		}
		field = _GetNextField(field);		
	}
	return nullptr;
}

void Form::_FindData_(std::vector<Data>& data, Field field_) {
	FormField* field = fields[0];
	while (field != nullptr) {
		if (field != nullptr && field->field == field_) 
			data.push_back(Data(field_, field->inputField));			
		field = _GetNextField(field);
	}
}

Frame::Coordinates Form::_GetSpecialContentCoord() {
	Frame::Coordinates coord = parentFrame->_GetCoordinates();
	specialContentHeight = (specialContentHeight > 0) ? specialContentHeight + 1 : 0;
	coord.x1 += padding;	
	coord.y1 += initialYpos + activeFields - hiddenFields + contentSpace + specialContentHeight;

	return coord;
}

Form::~Form() {
	for (int i = 0; i < fieldNum; i++)
		delete fields[i];
}

FormField::~FormField() {	
	if (inputField != nullptr) {
		if (inputField->parentFrame != nullptr) {
			delete inputField->parentFrame->dsp;
			inputField->parentFrame->_RemoveFrame();
			delete inputField->parentFrame;
		}
		delete inputField;
	}
}

OptionField::~OptionField() {
	for (int i = 0; i < optFieldNum; i++)
		delete optionalFields[i]->inputField;
}

FormField::FormField(const FormField& copy) : Label(copy.text) {
	this->type = copy.type;
	this->parentForm = copy.parentForm;
	this->mandatory = copy.mandatory;
	this->activated = copy.activated;
	this->filled = copy.filled;
	this->dataField = copy.dataField;
	this->inputField = copy.inputField;
	this->field =copy.field;
	this->OComponent::componentType = copy.OComponent::componentType;
}

FormField* FormField::_Store() {
	return new FormField(*this);
}

UsernameField::UsernameField(const UsernameField& copy) : FormField(copy), controller(copy.controller){
	this->OComponent::componentType = ComponentType::usernameField;
}

FormField* UsernameField::_Store() {
	return new UsernameField(*this);
}

OptionField::OptionField(const OptionField& copy) : FormField(copy) {
	this->optFieldNum = copy.optFieldNum;
	this->optionalFields = new FormField*[optFieldNum];
	for (int i = 0; i < optFieldNum; i++) 
		optionalFields[i] = copy.optionalFields[i]->_Store();	
	this->enabled = copy.enabled;
	this->OComponent::componentType = ComponentType::optionField;
}

FormField* OptionField::_Store() {
	return new OptionField(*this);
}

PasswordField::PasswordField(const PasswordField& copy) : FormField(copy) {
	this->master = copy.master;
	this->keyField = copy.keyField;
	this->OComponent::componentType = ComponentType::passwordField;
}

FormField* PasswordField::_Store() {
	return new PasswordField(*this);
}

FormField* SelectionField::_Store() {
	return new SelectionField(*this);
}

ConfirmField::ConfirmField(const ConfirmField& copy) : FormField(copy) {
	this->OComponent::componentType = ComponentType::confirmField;
}

FormField* ConfirmField::_Store() {
	return new ConfirmField(*this);
}

void Form::_Hide() {
	for (int i = 0; i < fieldNum; i++)
		fields[i]->_Hide();	
	contentSpace = 0;
}

void FormField::_Hide() {
	Label::_Hide();
	this->inputField->_HideInput();
	this->inputField->parentFrame->dsp->_WipeContent();
	if (!hidden && activated) {
		hidden = true;
		parentForm->_UpdateHiddenFields(1);
	}
}

void OptionField::_Hide() {
	FormField::_Hide();
	for (int i = 0; i < optFieldNum; i++) 
		optionalFields[i]->_Hide();
}

FormField* Form::_SelectField(Field field_) {
	FormField* field = fields[0];
	do {
		if (field->field == field_)
			return field;
		field = _GetNextField(field);		
	} while (field != nullptr);
	return nullptr;
}

std::vector<FormField*> Form::_SelectFields(std::vector<Field> fields_) {
	std::vector<FormField*> result;	
	for (size_t i = 0; i < fields_.size(); i++) {
		FormField* field = fields[0];
		do {
			if (field->field == fields_.at(i))
				result.push_back(field);
			field = _GetNextField(field);
		} while (field != nullptr);
	}
	return result;
}

void Form::_AddEvent(std::function<void(Form&, FormField*)> const& lambda) {
	std::function<void(Form&, FormField*)>* temp1 = new std::function<void(Form&, FormField*)>[eventNum + 1];
	bool* temp2 = new bool[eventNum + 1];
	for (int i = 0; i < eventNum; i++) {
		temp1[i] = events[i];
		temp2[i] = eventStatus[i];
	}
	delete[]events;
	delete[]eventStatus;
	temp1[eventNum] = lambda;
	temp2[eventNum] = false;
	events = temp1;
	eventStatus = temp2;

	eventNum++;
	eventEnabled = true;
}

void Form::_RemoveFields(int index, int fieldNum) {
	for (int i = 0; i < fieldNum; i++) {
		if (fields[index]->_GetActiveStatus())
			activeFields--;
		fields[index]->_Hide();
		delete fields[index];
		utility::_RemoveElement(fields, index, this->fieldNum);	
		_UpdateHiddenFields(-1);
	}	
}

void Form::_RunEvents(FormField* currentField) {
	lastField = currentField;
	for (int i = 0; i < eventNum; i++) 
		events[i](*this, currentField);		
}

bool Form::_SwitchToExtension(const char* moduleName) {
	this->moduler->_OpenModule(moduleName, moduler->_CurrentModule());
	return moduler->_CurrentModule()->_ExtensionStatus();
}

FormField* DateField::_Store() {
	return new DateField(*this);
}

DateField::DateField(const DateField& copy) : FormField(copy) {
	this->OComponent::componentType = ComponentType::dateField;
}

void DateField::_Show() {
	Label::_Show();
	if (currentDateDisplayed == false){		
		this->inputField->_SetDefaultInput(::currentDate);
		currentDateDisplayed = true;
	}
	if (_InputControl()) {
		filled = true;
		parentForm->_ShowNextField(this);
	}
	else _SwitchField(inputField->control);
}

Form::Form(const Form& copy) {
	this->status = copy.status;
	this->activeFields = copy.activeFields;
	this->hiddenFields = copy.hiddenFields;
	this->fieldNum = copy.fieldNum;	
	this->message = copy.message;
	this->paused = copy.paused;
	this->lastField = copy.lastField;
	this->specialContentHeight = copy.specialContentHeight;
	this->initialYpos = copy.initialYpos;
	this->contentSpace = copy.contentSpace;
	this->exit = copy.exit;	
	this->eventStatus = copy.eventStatus;
	this->eventEnabled = copy.eventEnabled;
	this->eventNum = copy.eventNum;
	this->moduler = copy.moduler;

	this->fields = new FormField*[fieldNum];
	for (int i = 0; i < fieldNum; i++)
		fields[i] = copy.fields[i]->_Store();

	this->events = new std::function<void(Form&, FormField*)>[eventNum];
	for (int i = 0; i < eventNum; i++)
		events[i] = copy.events[i];

	this->OComponent::componentType = ComponentType::form;
}