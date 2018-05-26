#pragma once
#include "IOComponent.h"

class Console : public OComponent {
private:
	char* _SystemMode();

	Console();
	~Console();

public:
	static Console& _GetInstance();
	Console(Console const&) = delete;
	void operator=(Console const&) = delete;

	void _Initialize();
	void _DrawFrame();
};