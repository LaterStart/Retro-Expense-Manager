#pragma once
#include <initializer_list>
#include <cstdarg> 

namespace utility {

	char* _ChopChar(char* arr, char* remove);
	char* _ChopChar(char* arr, char* removeOne, char* removeTwo);	
	char* _InsertChar(char* arr, char* insert);
	char* _ConvertToChar(unsigned int number);
	char* _ConcatenateChar(const char* myChar ...);
	char* _CopyChar(char* char_in);
	char* _CopyChar(const char* char_in);
	char* _GetCurrentDate();

	int _CharLength(char* arr);
	int _CharLength(const char* arr);
	int _CharSize(char* arr);
	int _Find(char* word, char* arr);
	int _ReadAlign(const char* align);
	int _ReadDirection(const char* direction);

	bool _CompareChar(char* first, char* second);
	bool _CompareChar(const char* first, const char* second);	

	template <typename T>
	void _AddElement(T* &ptr, T &element, int &arrSize) {
		T* newArr = new T[arrSize + 1];
		for (int i = 0; i < arrSize; i++)
			newArr[i] = ptr[i];
		newArr[arrSize] = element;
		delete[]ptr;
		ptr = newArr;
		arrSize++;
	}
	
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
		unsigned int n = unsigned int(a_args.size());
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


	template <class elementType>
	class LinkedList {
	public:
		elementType element;
		LinkedList* nextNode;
		LinkedList* previousNode;

		LinkedList(elementType element) { this->element = element; nextNode = nullptr; previousNode = nullptr; };
		LinkedList() { element = nullptr; nextNode = nullptr; previousNode = nullptr; };
		~LinkedList() {};

		void _AddNextLink(elementType element) {
			nextNode = new LinkedList(element);
			nextNode->previousNode = this;
		}

		void _RemoveLink() {
			if (nextNode == nullptr && previousNode == nullptr)
				return;
			else if (nextNode == nullptr)
				previousNode->nextNode = nullptr;
			else if (previousNode == nullptr)
				nextNode->previousNode = nullptr;
			else {
				previousNode->nextNode = nextNode;
				nextNode->previousNode = previousNode;
			}
		}
	};	

}
