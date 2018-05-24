#pragma once

class Cursor;
class Display {
private:
	unsigned short activePosNum;
	Cursor* activePositions;

	void _AddActivePosition(Cursor pos);
	void _ExtendRegister();

public:
	Display();
	~Display();
	
	void _ClearContent();
	void _DisplayContent(const char* content, Cursor &pos);	
	void _DisplayContent(const char* content);	
	void _LockContent(Cursor &pos);
};