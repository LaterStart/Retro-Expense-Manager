#include <fstream>
#include "CategoryController.h"
#include "../Models/_Header.h"
#include "../Models/Category.h"
using namespace std;

//	category types
vector<const char*> CategoryController::categoryType{ "Main Category", "Sub Category" };

//	static category model header
ModelHeader CategoryController::header(ModelName::category);

//	category controller constructor - loads categories from database
CategoryController::CategoryController() {
	this->model = ModelName::profile;
	if (this->header._Loaded() == false) {
		_LoadHeader(this->header);
	}
	this->_LoadCategoryList();
}

//	Add new category
void CategoryController::_AddNewCategory(utility::LinkedList<Data*>*data, int profileID) {
	Category newCategory(data, header._GiveID(), profileID);
	fstream* stream = _OpenStream();

	// check if model header exists
	if(this->header._Loaded() == false)
		this->_WriteNewModelHeader(stream, this->header);

	// write model		
	char* buffer = newCategory._Serialize();
	_WriteModel(stream, this->header, buffer);	

	// update 2D category list vector
	if (newCategory._Type() == CategoryType::mainCategory)
		categoryList->push_back(std::vector<Category>{newCategory});
	else if (newCategory._Type() == CategoryType::subCategory)
		categoryList->at(newCategory._ParentID()).push_back(newCategory);
	
	stream->close();
	delete stream;
}

//	Load categories from database into 2D category list vector
void CategoryController::_LoadCategoryList() {
	fstream* stream = _OpenStream();
	if (stream != nullptr) {
		categoryList = new std::vector<std::vector<Category>>;
		char** buffer = _GetModels(stream, this->header, Query(Range::all));
		for (unsigned int i = 0; i < header._NodeCount(); i++) {
			Category category(buffer[i]);
			if (category._Type() == CategoryType::mainCategory)
				categoryList->insert(categoryList->begin() + category._ID(), std::vector<Category>{category});
			else if (category._Type() == CategoryType::subCategory)
				categoryList->at(category._ParentID()).push_back(category);
			delete[]buffer[i];			
		}
		delete[]buffer;
		stream->close();
	}
	delete stream;
}

//	Returns only main categories from 2D vector as 1D vector
std::vector<Category> CategoryController::_GetMainCategoryList(){
	std::vector<Category> mainCategories;
	for (size_t i = 0; i < categoryList->size(); i++)
		mainCategories.push_back(categoryList->at(i).at(0));
	return mainCategories;
}