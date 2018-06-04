#include <iostream> //debug
#include <experimental/filesystem>
#include <filesystem> 
#include "ModuleController.h"
using namespace std;
namespace fs = experimental::filesystem::v1;

ModuleController::ModuleController(){}

ModuleController::~ModuleController(){}

//	read preconfigured modules file directory
utility::LinkedList<string>* ModuleController::_ReadModules() {	
	//	create linked list with all existing file names
	utility::LinkedList<string>* list = nullptr;	
	for (auto & p : fs::directory_iterator(modulesDirectory)) {
		if (list == nullptr)
			list = new utility::LinkedList<string>(p.path().string());
		else {
			list->_AddNextLink(p.path().string());
			list = list->nextNode;
		}
	}
	utility::LinkedList<string>* temp = nullptr;
	do {
		//	find and return list with only files under .h extension (classes)		
		char* className = utility::_ChopChar((char*)list->element.c_str(), (char*)modulesDirectory, (char*)".h");
		if (className != nullptr) {
			list->element = className;
			delete[]className;
		}
		else {
			temp = list;
			list->_RemoveLink();
		}
		if(list->previousNode != nullptr)
			list = list->previousNode;
		else {
			if (className == nullptr)				
				list = list->nextNode;
			delete temp;
			break;
		}
		delete temp;
		temp = nullptr;
	} while (true);
	
	return list;
}