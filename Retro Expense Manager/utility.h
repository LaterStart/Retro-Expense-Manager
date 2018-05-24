#pragma once
#include <initializer_list>
#include <stdarg.h> 

namespace utility {

	char* _ConvertToChar(unsigned int number);
	char* _ConcatenateChar(const char* myChar ...);
	char* _CopyChar(char* char_in);
	char* _CopyChar(const char* char_in);
	char* _GetCurrentDate();

	int _CharLength(char* arr);
	int _CharLength(const char* arr);
	int _CharSize(char* arr);
	
	template <typename T>
	void _RemoveElement(T* &ptr, int pos, int arrSize) {
		T* newArr = new T[arrSize - 1]; int z = 0;
		for (int i = 0; i < arrSize; i++) {
			if (i != pos)
				newArr[z++] = ptr[i];
		}
		delete[]ptr;
		ptr = newArr;
	}	

	template<typename T>
	void _CreateArray(T*& ptr, std::initializer_list<T> a_args) {
		unsigned int n = (unsigned int)a_args.size();
		ptr = new T[n];
		
		for (auto i : a_args)
			ptr[n] = i;	
	}	


	template <class elementType>
	class ElementsList	{
	private:
		class Link {
		public:
			elementType node;
			Link* nextNode;

			Link() : node(nullptr), nextNode(nullptr) {}
		};

		Link* first;
		Link* last;	
		Link* currentNode;
		unsigned int size;

	public:
		elementType node;
		ElementsList(elementType element ...) :first(nullptr), last(nullptr), node(nullptr), currentNode(0), size(0){
			va_list list;
			Link* link = new Link;
			first = link;
			for (va_start(list, element); element != 0; element = va_arg(list, elementType)) {
				size++;				
				link->node = element;
				link->nextNode = new Link;
				last = link;
				link = link->nextNode;				
			}
			node = first->node;
			currentNode = first;
		}

		unsigned int _GetSize() const { return size; }
		void _NextNode() {
			currentNode = currentNode->nextNode;
			node = currentNode->node;
		}
	};
}
