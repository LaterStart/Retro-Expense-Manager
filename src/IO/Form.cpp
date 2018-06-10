#include "Input.h"
#include "../utility.h"

void OptionField::_AddField(FormField& field) {
	FormField** pp = new FormField*(nullptr);
	utility::_AddElement(optionalFields, *pp, optFieldNum);
	optionalFields[optFieldNum - 1] = &field;
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

void FormField::_Show() {
	Label::_Show();
	if (!activated) {
		parentForm->_UpdateActiveFields(1);
		activated = true;
	}
	if (inputField != nullptr) {
		if (!filled) {
			inputField->_ReadUserInput();
			filled = true;
			parentForm->_ShowNextField(this);


		}
	}
}

void OptionField::_Show() {
	Label::_Show();
	if (!activated) {
		parentForm->_UpdateActiveFields(1);
		activated = true;
	}
	if (inputField != nullptr) {
		if (!filled) {
			inputField->_ReadUserInput();
			filled = true;		
			parentForm->_UpdateActiveFields(1);
		}

		if (inputField->check == true) {
			if (enabled = false)
				parentForm->_EnableOptional(optFieldNum);
			for (int i = 0; i < optFieldNum; i++)
				optionalFields[i]->_Show();
		}
		else if (enabled) {
			parentForm->_DisableOptional(optFieldNum);
			for (int i = 0; i < optFieldNum; i++)
				if (optionalFields[i]->_ActiveStatus() == true)
					parentForm->_UpdateActiveFields(-1);

			enabled = false;
		}
		parentForm->_ShowNextField(this);
	}
}

FormField* Form::_GetNextField(FormField* currentField) const {
	for (int i = 0; i < fieldNum; i++) 
		if (fields[i]->_GetNextField(currentField) != nullptr)
			return fields[i]->_GetNextField(currentField);
	return nullptr;
}

FormField* OptionField::_GetNextField(FormField* currentField) {
	for (int i = 0; i < optFieldNum; i++)
		if (optionalFields[i] == currentField) {
			if(i+1 < optFieldNum)
				return optionalFields[i + 1];
			else return nullptr;
		}			
	return nullptr;
}

void PasswordField::_Show() {
	Label::_Show();
	if (!activated) {
		parentForm->_UpdateActiveFields(1);
		activated = true;
	}
	if (inputField != nullptr) {
		if (!filled) {
			inputField->_ReadUserInput();
			filled = true;
			parentForm->_UpdateActiveFields(1);

			if (master) {
				PasswordField* slave = dynamic_cast<PasswordField*>(parentForm->_GetNextField(this));
				slave->_SetKeyField(this);
				parentForm->_ShowNextField(this);
			}
			else if (keyField != nullptr)
				_VerifyPassword();			
		}
	}
}

void PasswordField::_VerifyPassword() {
	if (utility::_CompareChar(inputField->input, keyField->inputField->input) == false) {
		keyField->inputField->_ClearInput();
		keyField->filled = false;
		this->inputField->_ClearInput();
		this->filled = false;
		parentForm->_ChangeCurrentField(-1);
		parentForm->_DisplayMessage("Passwords didn't match.");
	}
	else {
		filled = true;
		parentForm->_ClearMessage();
		parentForm->_ShowNextField(this);
	}
}

void Form::_DisplayMessage(const char* message) {
	Frame::Coordinates coord = parentFrame->_GetCoordinates();
	coord.x1 += padding;
	coord.y1 += activeFields+2;
	Cursor mssgPos(coord.x1, coord.y1);
	this->message._Display(mssgPos, message);
}

void Form::_EnableOptional(int optFieldNum) {
	for (int i = currField + 1; i < fieldNum; i++)
		fields[i]->_ShiftInputFrame(optFieldNum);
}

void Form::_DisableOptional(int optFieldNum) {
	for (int i = currField + 1; i < fieldNum; i++) 
		fields[i]->_ShiftInputFrame(optFieldNum*-1);	
}

void FormField::_ShiftInputFrame(int num) {
	Ypos += num;
	Frame::Coordinates coord = parentFrame->_Select(text)->_GetCoordinates();
	coord.y1 += num;
	coord.y2 += num;
	parentFrame->_Select(text)->_ChangeCoordinates(coord);
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

void Form::_Show() {
	_InitializeFields();		
	_ShowNextField(nullptr);
}

void Form::_ShowNextField(FormField* currentField) {
	if (currentField == nullptr)
		fields[0]->_Show();
	else {
		for (int i = 0; i < fieldNum; i++) {
			if (fields[i] == currentField) {
				if (i + 1 < fieldNum) {
					fields[i + 1]->_Show();
					break;
				}
				else break;
			}
			else if (fields[i]->_GetNextField(currentField) != nullptr) {
				fields[i]->_GetNextField(currentField)->_Show();
				break;
			}
		}
	}
}

void Form::_ShowPreviousField(FormField* currentField) {
	for (int i = 0; i < fieldNum; i++) {
		if (fields[i] == currentField) {
			if (i > 0)
				fields[i - 1]->_Show();
			else fields[i]->_Show();
		}
		//else if(fields[i].get)
	}
}