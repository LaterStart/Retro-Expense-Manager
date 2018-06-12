#pragma once

class Profile {
	friend class ProfileController;
private:
	int id;
	int defCCYid;
	char* username;
	char* password;
	bool pwProtected;

public:

	


	Profile();
	~Profile();	
};
