#include "Category.h"
#include "../IO/Input.h"

// construct category model using form data
Category::Category(utility::LinkedList<Data*>* data, int ID, int profileID){
	while (true) {
		_BindData(data->element);
		if (data->nextNode == nullptr)
			break;
		else data = data->nextNode;
	}
	this->ID = ID;
	this->profileID = profileID;
	data->_DeleteList();
}

// construct category model using bufferered data
Category::Category(char* buffer) {
	this->name = buffer;
}

// category copy constructor
Category::Category(const Category& copy) {
	this->ID = copy.ID;
	this->parentID = copy.parentID;
	this->profileID = copy.profileID;
	this->name = copy.name;
	this->type = copy.type;	
}

//	binds form data to object data
void Category::_BindData(Data* data) {
	switch (data->field) {
	case Field::categoryName:
		name = utility::_CopyChar(data->input->input);	
		nameSize = utility::_CharLength(name);
		break;
	case Field::categoryType:
		type = static_cast<CategoryType>(data->input->selection - 1);
		break;
	case Field::parentCategory:
		parentID = data->input->selection;
		break;	
	default:
		break;
	}
}

//	serialize category model
char* Category::_Serialize() {
	//	Total object size					 
	int size = nameSize + 3*sizeof(int);

	//	insert object size info and ID at buffer start
	char* buffer = new char[size+2*sizeof(int)];
	char* firstByte = buffer;	

	std::memcpy(buffer, &ID, sizeof(int));
	buffer += sizeof(int);
	std::memcpy(buffer, &size, sizeof(int));	
	buffer += sizeof(int);

	//	store IDs into buffer
	int* ptr[] = { &profileID, &parentID };
	for (int i = 0; i < sizeof(ptr) / sizeof(ptr[0]); i++) {
		std::memcpy(buffer, *&ptr[i], sizeof(int));
		buffer += sizeof(int);
	}

	//	store name into buffer
	std::memcpy(buffer, &nameSize, sizeof(int));
	buffer += sizeof(int);
	std::memcpy(buffer, name, nameSize);

	return firstByte;
}

//	deserialize category model
void Category::_Deserialize(char* page) {
	this->ID = *(int*)page;
	page += sizeof(int);

	//	deserialize IDs
	int* ptr[] = { &profileID, &parentID};
	for (int i = 0; i < sizeof(ptr) / sizeof(ptr[0]); i++) {
		*ptr[i] = *(int*)page;
		page += sizeof(int);
	}
	
	this->nameSize = *(int*)page;
	page += sizeof(int);

	this->name = new char[nameSize];
	std::memcpy(name, page, nameSize);
}

std::ostream& Category::_Show(std::ostream& os) {
	os << this->name;
	return os;
}

Category::~Category() {
	//delete[]name;
}