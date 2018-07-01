#include <fstream>
#include "CategoryController.h"
#include "../Models/_Header.h"
#include "../Models/Category.h"
using namespace std;

//	Categories list vector
vector<Category> categoryList = { Category("one"), Category("two"), Category("three"), Category("four"), Category("five"), Category("six"), Category("seven"), Category("eight") };

//	static category model header
ModelHeader CategoryController::header(ModelName::transaction);

//	category controller constructor
CategoryController::CategoryController() {}

//	Add new transaction
void CategoryController::_AddNewCategory(utility::LinkedList<Data*>*data, int profileID) {
	Category newCategory(data, header._GiveID(), profileID);
	fstream* stream = _OpenStream();

	// write model		
	char* buffer = newCategory._Serialize();
	_WriteModel(stream, this->header, buffer);	
	
	stream->close();
	delete stream;
}