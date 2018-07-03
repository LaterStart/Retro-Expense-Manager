#pragma once
#include "_Model.h"

class Category : public Model {
private:	
	int ID = -1;
	int profileID = -1;
	char* name = nullptr;
	
	void _BindData(Data* data);
	std::ostream& _Show(std::ostream& os);

public:	
	char* _Serialize();
	void _Deserialize(char* buffer);	
	
	int _ID() const;
	char* _Name() const;
	int _DisplayLength();

	Category() = default;
	Category(utility::LinkedList<Data*>* data, int ID, int profileID);
	Category(char* buffer);
	Category(const Category& copy);
	~Category();	
};

inline int Category::_ID() const {
	return this->ID;
}

inline char* Category::_Name() const {
	return this->name;
}

inline int Category::_DisplayLength() {
	return utility::_CharLength(name);
}