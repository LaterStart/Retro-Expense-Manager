#pragma once
#include "_Model.h"

class Profile : public Model {
private:	
	int ID = -1;
	int defaultCurrencyID = -1;
	int usernameSize = 0;
	int passwordSize = 0;
	char* username = nullptr;	
	char* password = nullptr;
	bool pwProtected = false;
	bool active = true;
	bool locked = true;
	
	void _BindData(Data* data);
	std::ostream& _Show(std::ostream& os);

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
	void _Unlock();
	bool _Locked() const;
	int _DisplayLength();

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

inline void Profile::_Unlock() {
	this->locked = false;
}

inline bool Profile::_Locked() const {
	return this->locked;
}

inline int Profile::_DisplayLength() {
	return utility::_CharLength(username);
}