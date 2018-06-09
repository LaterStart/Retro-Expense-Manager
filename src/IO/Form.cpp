#include "Input.h"
#include "../utility.h"

void Form::_AddField(FormField& field) {
	FormField** pp = new FormField*(nullptr);
	utility::_AddElement(fields, *pp, fieldNum);
	fields[fieldNum - 1] = &field;
	delete pp;
}

void OptionField::_AddField(FormField& field) {
	FormField** pp = new FormField*(nullptr);
	utility::_AddElement(optionalFields, *pp, optFieldNum);
	optionalFields[optFieldNum - 1] = &field;
	delete pp;
}

void OptionField::_Show() {
	FormField::_Show();
	if(condition)
		for (int i = 0; i < optFieldNum; i++) {
			optionalFields[i]->_SetParentFrame(parentFrame);
			optionalFields[i]->_SetYpos(parentFrame->nextYpos++);
			optionalFields[i]->_SetPadding(padding);
			optionalFields[i]->_Show();
		}
}

void OptionField::_ActivateOptional() {
	this->condition = true;
}

void Form::_InitializeFields() {
	for (int i = 0; i < fieldNum; i++) {
		fields[i]->_SetParentFrame(parentFrame);
		fields[i]->_SetPadding(padding);
	}
}

void Form::_Show() {
	_InitializeFields();
	do {
		for (int i = 0; i <= filledNum; i++) {
			fields[i]->_SetYpos(parentFrame->nextYpos++);
			fields[i]->_Show();
		}	
		
		


		UserInput input(InputType::text);
		input._ReadUserInput();


		filledNum++;
		parentFrame->nextYpos = 0;
	} while (filledNum < fieldNum);
}