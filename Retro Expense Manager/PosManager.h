#pragma once

class PosManager {
protected:
	unsigned short charNum;

public:
	static unsigned short activePosNum;
	virtual void _SetCharacterNumber(unsigned short value) = 0;
};


