#pragma once
#include "_Model.h"

class Profile : public Model {
private:
	int id = -1;
	int defCCYid = -1;
	int usernameSize = 0;
	int passwordSize = 0;
	char* username = nullptr;	
	char* password = nullptr;
	bool pwProtected = false;

	void _BindData(Data* data);

public:
	char* _Serialize();
	void _Deserialize(std::ifstream* stream);

	Profile(utility::LinkedList<Data*>* data);
	~Profile() = default;
};
