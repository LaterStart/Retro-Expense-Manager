#pragma once
#include "_Model.h"

class Category : public Model {
private:	
	int ID = -1;
	int parentID = -1;
	int profileID = -1;	
	int nameSize = 0;
	char* name = nullptr;
	CategoryType type = CategoryType::mainCategory;
	
	void _BindData(Data* data);
	std::ostream& _Show(std::ostream& os);

public:	
	char* _Serialize();
	void _Deserialize(char* buffer);	
	
	int _ID() const;
	int _ParentID() const;
	char* _Name() const;
	CategoryType _Type() const;
	int _DisplayLength();
	void _SetProfileID(int profileID);
	void _SetID(int ID);

	Category() = default;
	Category(utility::LinkedList<Data*>* data, int ID, int profileID);
	Category(char* buffer);
	Category(const char* name, CategoryType type = CategoryType::mainCategory);
	Category(const Category& copy);
	Category(Category&& move);
	~Category();	

	Category& operator=(Category&& move);
};

inline int Category::_ID() const {
	return this->ID;
}

inline char* Category::_Name() const {
	return this->name;
}

inline CategoryType Category::_Type() const {
	return this->type;
}

inline int Category::_DisplayLength() {
	return nameSize;
}

inline int Category::_ParentID() const {
	return this->parentID;
}

inline void Category::_SetProfileID(int profileID) {
	this->profileID = profileID;
}

inline void Category::_SetID(int ID) {
	this->ID = ID;
}