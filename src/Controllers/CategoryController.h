#pragma once
#include "_Controller.h"

class Category; class ModelHeader;
class CategoryController : public Controller {
private:
	static ModelHeader header;	

public:
	CategoryController();
	~CategoryController() = default;

	static std::vector<const char*> categoryType;
	static std::vector<std::vector<Category>> categoryList;
	
	void _AddNewCategory(utility::LinkedList<Data*>*data, int profileID);
};