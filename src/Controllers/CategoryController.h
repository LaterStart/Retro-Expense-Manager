#pragma once
#include "_Controller.h"

class Category; class ModelHeader;
class CategoryController : public Controller {
private:
	static ModelHeader header;
	

public:
	CategoryController();
	~CategoryController() = default;
	
	void _AddNewCategory(utility::LinkedList<Data*>*data, int profileID);
};

extern std::vector<Category> categoryList;