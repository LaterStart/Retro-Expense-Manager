#pragma once
#include "../Controllers/_Controller.h"
#include "../utility.h"

class Model {	
protected:
	virtual char* _Serialize() = 0;
	virtual void _Deserialize(char* page) = 0;
};