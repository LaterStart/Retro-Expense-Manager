#pragma once
#include "_Model.h"

class Header : public Model {
protected:
	int nodeCount = 0;
	int nextNodePage = -1;
	std::streamoff pos = -1;
};

class MainHeader : public Header {
private:
	bool valid = false;

public:
	char* _Serialize();
	void _Deserialize(char* stream);

	MainHeader() = default;
	~MainHeader() = default;
};