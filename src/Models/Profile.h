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
	void _Deserialize(char* page);

	char* _Username() const;

	Profile(utility::LinkedList<Data*>* data);
	Profile(char* buffer);
	~Profile() = default;
};

inline char* Profile::_Username() const {
	return this->username;
}
