#pragma once
#include "_Model.h"

class Profile : public Model {
private:	
	int ID = -1;
	int defCCYid = -1;
	int usernameSize = 0;
	int passwordSize = 0;
	char* username = nullptr;	
	char* password = nullptr;
	bool pwProtected = false;
	bool active = true;

	void _BindData(Data* data);

public:
	char* _Serialize();
	void _Deserialize(char* buffer);

	char* _Username() const;
	bool _PwStatus() const;
	bool _Active() const;
	void _Activate();
	void _Deactivate();
	int _ID() const;
	bool _VerifyPassword(char* password);

	Profile(utility::LinkedList<Data*>* data, int ID);
	Profile(char* buffer);
	Profile(const Profile& copy);
	~Profile();
};

inline char* Profile::_Username() const {
	return this->username;
}

inline bool Profile::_Active() const {
	return this->active;
}

inline void Profile::_Activate() {
	this->active = true;
}

inline void Profile::_Deactivate() {
	this->active = false;
}

inline int Profile::_ID() const {
	return this->ID;
}

inline bool Profile::_PwStatus() const {
	return this->pwProtected;
}