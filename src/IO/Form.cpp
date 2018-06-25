#include "Input.h"
#include "../Controllers/ProfileController.h"
#include "../utility.h"

void Form::_AddField(FormField& field) {
	FormField** pp = new FormField*(nullptr);
	utility::_AddElement(fields, *pp, fieldNum);
	fields[fieldNum - 1] = field._Store();
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
	inputField = new UserInput(type);
	inputField->_SetParentFrame(parentFrame->_Select(text));
}

void OptionField::_CreateInputFrame() {
	FormField::_CreateInputFrame();
	for (int i = 0; i < optFieldNum; i++) {
		optionalFields[i]->_SetParentFrame(parentFrame);
		optionalFields[i]->_SetPadding(padding);
		optionalFields[i]->_SetYpos(parentFrame->nextYpos++);		
		optionalFields[i]->_CreateInputFrame();	
		optionalFields[i]->_SetParentForm(parentForm);
	}
}

void FormField::_ShiftInputFrame(int num) {
	Ypos += num;
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
	for (int i = 0; i < fieldNum; i++) {
		fields[i]->_SetParentFrame(parentFrame);
		fields[i]->_SetPadding(padding);
		fields[i]->_SetYpos(parentFrame->nextYpos++);
		fields[i]->_SetParentForm(this);
		fields[i]->_CreateInputFrame();
	}
}

bool FormField::_InputControl() {
	if (!activated) {
		parentForm->_UpdateActiveFields(1);
		activated = true;
	}
	inputField->_ReadUserInput();

	if (inputField->control == 1)
		return false;
	else if (inputField->control == 2)
		return false;
	else if (inputField->control == -1)
		return false;
	else if (inputField->control == 3)
		return false;
	else {
		if (mandatory && inputField->length < 1) {
			parentForm->_DisplayMessage("This field cannot be blank.");
			return false;
		}
		parentForm->_ClearMessage();
		return true;
	}
}

void FormField::_SwitchField(int control) {
	switch (control) {
	case -1:
		parentForm->_Exit(this);
		break;
	case 1:
		parentForm->_ShowPreviousField(this);
		break;
	case 2:
		if(filled)
			parentForm->_ShowNextField(this);
		else this->_Show();
		break;
	case 3:
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
		if (inputField->control == 2 && controller._Exists(inputField->input)) {
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
		if (inputField->check == true) {
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
		if (inputField->control == 2 && keyField != nullptr) {
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

void ConfirmField::_Show() {
	Label::_Show();
	if (_InputControl()) {
		this->_Clear();
		activated = false;
		parentForm->_UpdateActiveFields(-1);
		if (inputField->check == true) {			
			parentForm->_DisplayMessage("Saved successfully.");
			parentForm->_SetStatus(true);
			Display* dsp = _GetDisplay();		
			dsp->_Loading();
		}
	}
	else {
		int control = inputField->control;
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

void FormField::_Hide() {
	Label::_Hide();
	this->inputField->_HideInput();
}

void Form::_DisplayMessage(const char* message) {
	_ClearMessage();
	Frame::Coordinates coord = parentFrame->_GetCoordinates();
	coord.x1 += padding;
	coord.y1 += activeFields + 1 + this->Ypos;
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
		nextField->_ShiftInputFrame(optFieldNum*-1);
		nextField = _GetNextField(nextField);		
	}
}

void Form::_Show() {
	if (paused) {
		paused = false;
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
	if(confirm.check == true)
		status = false;
	else {
		_ClearMessage();
		confirm._ClearInput();
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
	Data* data = new Data(fields[0]->dataType, fields[0]->inputField);
	utility::LinkedList<Data*>* list = new utility::LinkedList<Data*>(data);
	utility::LinkedList<Data*>* first = list;
	while (field != nullptr) {		
		field = _GetNextField(field);
		if (field != nullptr && field->_GetDataStatus()) {
			data = new Data(field->dataType, field->inputField);
			list->_AddNextLink(data);
			list = list->nextNode;
		}
	}
	return first;
}

Form::~Form() {
	for (int i = 0; i < fieldNum; i++)
		delete fields[i];
}

FormField::~FormField() {
	delete inputField;
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
	this->dataType =copy.dataType;
}

FormField* FormField::_Store() {
	return new FormField(*this);
}

UsernameField::UsernameField(const UsernameField& copy) : FormField(copy), controller(copy.controller){}

FormField* UsernameField::_Store() {
	return new UsernameField(*this);
}

OptionField::OptionField(const OptionField& copy) : FormField(copy) {
	this->optionalFields = copy.optionalFields;
	this->optFieldNum = copy.optFieldNum;
	this->enabled = copy.enabled;
}

FormField* OptionField::_Store() {
	return new OptionField(*this);
}

PasswordField::PasswordField(const PasswordField& copy) : FormField(copy) {
	this->master = copy.master;
	this->keyField = copy.keyField;
}

FormField* PasswordField::_Store() {
	return new PasswordField(*this);
}

ConfirmField::ConfirmField(const ConfirmField& copy) : FormField(copy) {}

FormField* ConfirmField::_Store() {
	return new ConfirmField(*this);
}