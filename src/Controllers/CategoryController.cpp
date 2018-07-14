#include <fstream>
#include "CategoryController.h"
using namespace std;

//	category types
vector<const char*> CategoryController::categoryType{ "Main Category", "Sub Category" };

//	static category model header
ModelHeader CategoryController::header(ModelName::category);

//	category controller constructor - loads categories from database
CategoryController::CategoryController() {
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
		categoryList->push_back(vector<Category>{newCategory});
	else if (newCategory._Type() == CategoryType::subCategory) {
		for (size_t i = 0; i < categoryList->size(); i++) {
			if (categoryList->at(i).at(0)._ID() == newCategory._ParentID())
				categoryList->at(i).push_back(newCategory);
		}
	}		
	
	stream->close();
	delete stream;
}

void CategoryController::_AddNewCategory(Category& newCategory, int profileID) {
	newCategory._SetID(header._GiveID());
	newCategory._SetProfileID(profileID);
	fstream* stream = _OpenStream();

	// check if model header exists
	if (this->header._Loaded() == false)
		this->_WriteNewModelHeader(stream, this->header);

	// write model		
	char* buffer = newCategory._Serialize();
	_WriteModel(stream, this->header, buffer);

	// update 2D category list vector
	if (newCategory._Type() == CategoryType::mainCategory)
		categoryList->push_back(vector<Category>{newCategory});
	else if (newCategory._Type() == CategoryType::subCategory) {
		for (size_t i = 0; i < categoryList->size(); i++) {
			if (categoryList->at(i).at(0)._ID() == newCategory._ParentID())
				categoryList->at(i).push_back(newCategory);
		}
	}

	stream->close();
	delete stream;
}

//	Load categories from database into 2D category list vector
void CategoryController::_LoadCategoryList() {
	categoryList = new vector<vector<Category>>;
	fstream* stream = _OpenStream();	
	if (stream != nullptr) {		
		vector<char*>* buffer = _GetModels(stream, this->header, Query(Range::all));
		if (buffer != nullptr) {
			for (unsigned int i = 0; i < buffer->size(); i++) {
				Category category(buffer->at(i));
				if (category._Type() == CategoryType::mainCategory)
					categoryList->push_back(vector<Category>{category});
				else if (category._Type() == CategoryType::subCategory) {
					for (size_t i = 0; i < categoryList->size(); i++) {
						if (categoryList->at(i).at(0)._ID() == category._ParentID())
							categoryList->at(i).push_back(category);
					}
				}
				delete[]buffer->at(i);
			}
			delete buffer;
		}
		stream->close();
	}
	delete stream;
}

//	Returns only main categories from 2D vector as 1D vector
vector<Category> CategoryController::_GetMainCategoryList(){
	vector<Category> mainCategories;
	for (size_t i = 0; i < categoryList->size(); i++) {
		Category* category = &categoryList->at(i).at(0);
		if(category->_Type() == CategoryType::mainCategory)
			mainCategories.push_back(*category);
	}
	return mainCategories;
}