#pragma once
#include "_Controller.h"
#include "../Models/Category.h"

class Category; class ModelHeader;
class CategoryController : public Controller {
private:
	static ModelHeader header;	

	void _LoadCategoryList();
public:
	CategoryController();
	~CategoryController() = default;

	static std::vector<const char*> categoryType;
	std::vector<std::vector<Category>>* categoryList;
	
	void _AddNewCategory(utility::LinkedList<Data*>*data, int profileID);
	void _AddNewCategory(Category& category, int profileID);	
	std::vector<Category> _GetMainCategoryList();
	bool _Exists(char* name);
	Category* _GetCategory(int id);
};