#include "Profile.h"

Profile::Profile() : username(nullptr), password(nullptr), locked(false){}

Profile::~Profile(){
	delete[]username;
	delete[]password;
}