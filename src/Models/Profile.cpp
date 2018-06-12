#include "Profile.h"

Profile::Profile() : username(nullptr), password(nullptr), pwProtected(false){}

Profile::~Profile(){
	delete[]username;
	delete[]password;
}