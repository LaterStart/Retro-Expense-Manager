#pragma once
#include "IOComponent.h"

class Console;
class Initialize {
private:
	Initialize() = delete;
	~Initialize() = delete;

	static bool initialized;

public:
	static Console* _Console();
};

class Frame;
class Console : public OComponent {
private:
	char* _SystemMode();

	Console();
	~Console();

	Frame* mainFrame;

	void _Initialize();
	void _DrawFrame();
	static Console& _GetInstance();

	friend Console* Initialize::_Console();

public:	
	Console(Console const&) = delete;
	void operator=(Console const&) = delete;

	Frame* _GetMainFrame() const;
};

inline Frame* Console::_GetMainFrame() const {
	return mainFrame;
}