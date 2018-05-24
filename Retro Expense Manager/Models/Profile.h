#pragma once

class Profile {
private:
	char* username;
	char* password;
	bool locked;

public:
	Profile();
	~Profile();	
};
