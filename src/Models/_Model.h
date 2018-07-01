#pragma once
#include "../Controllers/_Controller.h"
#include "../utility.h"

class Model {	
protected:
	virtual char* _Serialize() = 0;
	virtual void _Deserialize(char* page) = 0;	
	virtual std::ostream& _Show(std::ostream& os) = 0;

public:
	friend std::ostream& operator<<(std::ostream& os, Model& model);
	virtual int _DisplayLength() = 0;
};

inline std::ostream& operator<<(std::ostream& os, Model& model) {
	return model._Show(os);
}