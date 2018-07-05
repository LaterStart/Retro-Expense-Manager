#include <fstream>
#include "CategoryController.h"
#include "../Models/_Header.h"
#include "../Models/Category.h"
using namespace std;

//	category types
vector<const char*> CategoryController::categoryType{ "Main Category", "Sub Category" };

//	category list 2D vector
std::vector<std::vector<Category>> CategoryController::categoryList =
{ std::vector<Category>{Category("glavna 01"), Category("sub 01-01"), Category("sub 01-02"), Category("sub 01-03"), Category("sub 01-04"), Category("sub 01-05"), Category("sub 01-06")},
  std::vector<Category>{Category("glavna 02"), Category("sub 02-01"), Category("sub 02-02"), Category("sub 02-03"), Category("sub 02-04"), Category("sub 02-05"), Category("sub 02-06")},
  std::vector<Category>{Category("glavna 03"), Category("sub 03-01"), Category("sub 03-02"), Category("sub 03-03"), Category("sub 03-04"), Category("sub 03-05"), Category("sub 03-06")},
  std::vector<Category>{Category("glavna 04"), Category("sub 04-01"), Category("sub 04-02"), Category("sub 04-03"), Category("sub 04-04"), Category("sub 04-05"), Category("sub 04-06")},
  std::vector<Category>{Category("glavna 05"), Category("sub 05-01"), Category("sub 05-02"), Category("sub 05-03"), Category("sub 05-04"), Category("sub 05-05"), Category("sub 05-06")},
  std::vector<Category>{Category("glavna 06"), Category("sub 06-01"), Category("sub 06-02"), Category("sub 06-03"), Category("sub 06-04"), Category("sub 06-05"), Category("sub 06-06")},
};

//	main category list 1D vector
std::vector<Category> CategoryController::mainCategoryList;

//	static category model header
ModelHeader CategoryController::header(ModelName::category);

//	category controller constructor
CategoryController::CategoryController() {}

//	updates main category list using category list vector content
void CategoryController::_UpdateMainCategoryList() {
	mainCategoryList.clear();
	for (size_t i = 1; i < categoryList.size(); i++)
		mainCategoryList.push_back(categoryList[i].at(0));
}

//	Add new transaction
void CategoryController::_AddNewCategory(utility::LinkedList<Data*>*data, int profileID) {
	Category newCategory(data, header._GiveID(), profileID);
	fstream* stream = _OpenStream();

	// write model		
	char* buffer = newCategory._Serialize();
	_WriteModel(stream, this->header, buffer);	

	// update category list vector
	if (newCategory._Type() == CategoryType::mainCategory) {
		std::vector<Category> newMain{ newCategory };
		categoryList.push_back(newMain);	
	}
	else if (newCategory._Type() == CategoryType::subCategory)
		categoryList.at(newCategory._ParentID() + 1).push_back(newCategory);
	
	stream->close();
	delete stream;
}