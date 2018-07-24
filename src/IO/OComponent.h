#pragma once
#include "IOComponent.h"
#include "../config.h"

class OComponent : public IOComponent {	
private:
	void _SetInitialCoordinates();
protected:
	OComponent(const unsigned short min_x, const unsigned short max_x, const unsigned short min_y, const unsigned short max_y);
	OComponent(const OComponent& copy);
	OComponent(OComponent* outOfFrame);

	const unsigned short min_x;
	const unsigned short max_x;
	const unsigned short min_y;
	const unsigned short max_y;

	unsigned short x1, x2;
	unsigned short y1, y2;	

public:
	struct Coordinates : public IOComponent {
		unsigned short x1, x2;
		unsigned short y1, y2;

		void operator=(const Coordinates& equals) {
			x1 = equals.x1;
			x2 = equals.x2;
			y1 = equals.y1;
			y2 = equals.y2;
		}
		Coordinates() {
			this->componentType = ComponentType::coordinates;
		}
	};	

protected:
	Coordinates coord;

public:
	void _SetX1(unsigned short);
	void _SetX2(unsigned short);
	void _SetY1(unsigned short);
	void _SetY2(unsigned short);
	void _SetX(unsigned short, unsigned short);
	void _SetY(unsigned short, unsigned short);	
	unsigned short _X1() const;
	unsigned short _X2() const;
	unsigned short _Y1() const;
	unsigned short _Y2() const;
	unsigned short _Height() const;
	unsigned short _Width() const;
	short _GetArea();

	Coordinates _GetCoordinates();
	~OComponent() = default;
};

inline void OComponent::_SetX1(unsigned short x1) {
	this->x1 = x1;
}

inline void OComponent::_SetX2(unsigned short x2) {
	this->x2 = x2;
}

inline void OComponent::_SetY1(unsigned short y1) {
	this->y1 = y1;
}

inline void OComponent::_SetY2(unsigned short y2) {
	this->y2 = y2;
}

inline void OComponent::_SetX(unsigned short x1, unsigned short x2) {
	this->x1 = x1;
	this->x2 = x2;
}

inline void OComponent::_SetY(unsigned short y1, unsigned short y2) {
	this->y1 = y1;
	this->y2 = y2;
}

inline OComponent::Coordinates OComponent::_GetCoordinates()  {
	coord.x1 = x1;
	coord.x2 = x2;
	coord.y1 = y1;
	coord.y2 = y2;
	return coord;
}

inline short OComponent::_GetArea() {
	return (x2 - x1) * (y2 - y1);
}

inline unsigned short OComponent::_X1() const {
	return this->x1;
}

inline unsigned short OComponent::_X2() const {
	return this->x2;
}

inline unsigned short OComponent::_Y1() const {
	return this->y1;
}

inline unsigned short OComponent::_Y2() const {
	return this->y2;
}

inline unsigned short OComponent::_Height() const {
	return y2 - y1;
}

inline unsigned short OComponent::_Width() const {
	return x2 - x1;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------

class FrameElement;
class Frame : public OComponent{	
	friend class Console;
	friend class Layout;
	friend class Table;

private:	
	Frame(const unsigned short max_x, const unsigned short min_x, const unsigned short max_y, const unsigned short min_y);
	Frame(Frame& parentFrame, const unsigned short max_x, const unsigned short min_x, const unsigned short max_y, const unsigned short min_y);	
	void _AddFrame(Frame* newFrame);
	void _RemoveFrame(Frame* frame);
	unsigned short leftPadding = 0;

	void _AddElement(FrameElement& newElement);
public:
	struct Container : public IOComponent{
		Frame** frames = nullptr;
		int frameNum = 0;
		Container() {
			this->componentType = ComponentType::container;
		}
		Container(const Container& copy);
		~Container();		
	};

	short nextYpos = 0;
	const char* IDname = nullptr;
	Frame(Frame& parentFrame);
	Container _GetSubFrames();
	void _Split(Separator& separator, const char* firstID, const char* secondID);
	void _Split(unsigned short percent, const char* direction, const char* firstID, const char* secondID);
	void _CreateSubFrame(const char* IDname, Coordinates& coord);
	void _SetIDname(const char* IDname);
	void _SetDisplay(Display& dsp);
	void _ChangeCoordinates(Coordinates& coord);
	Frame* _Select(const char* IDname);
	Frame* _GetParentFrame();

	Frame * parentFrame = nullptr;
	Display* dsp = nullptr;
	FrameElement** elements = nullptr;
	int elNum = 0;		

	void _AddPadding(unsigned short padding);
	void _AddLeftPadding(unsigned short padding);
	void _AddRightPadding(unsigned short padding);
	void _AddTopPadding(unsigned short padding);
	void _AddBottomPadding(unsigned short padding);
	void _ShowElements();
	void _RemoveFrame();
	std::vector<FrameElement*> _SelectElements(ComponentType type);

	//	Add more FrameElements using variadic template
	template <typename T>
	void _AddElements(T& element) {
		_AddElement(element);
	}
	template<typename T , typename ... TT>
	void _AddElements(T& element, TT& ... nextElements ) {
		_AddElement(element);
		_AddElements(nextElements...);
	}

	~Frame();

private:
	Container container;
};

inline void Frame::_SetIDname(const char* IDname) {
	this->IDname = IDname;
}

inline Frame::Container Frame::_GetSubFrames() {
	return container;
}

inline Frame* Frame::_GetParentFrame() {
	return parentFrame;
}

inline void Frame::_SetDisplay(Display& dsp) {
	this->dsp = &dsp;
}

class FrameElement : public OComponent {
protected:
	FrameElement * original = nullptr;
	FrameElement * clone = nullptr;

	bool paddingSet = false;
	bool yposSet = false;
	const char* align = "left";
	unsigned short padding = 0;
	unsigned short Ypos = 0;
	Frame* parentFrame;
	FrameElement();
	FrameElement(Frame* parentFrame);	

	Display* _GetDisplay();
	virtual Cursor _Align();

public:
	FrameElement(const FrameElement& copy);
	FrameElement(const FrameElement& copy, Frame* parentFrame);
	void _SetParentFrame(Frame* parentFrame);
	
	virtual void _Show();
	virtual void _Hide();
	virtual void _SetYpos(unsigned short y);
	void _SetAlign(const char* align);	
	void _SetPadding(unsigned short padding);
	bool _PaddingSet() const;
	bool _YposSet() const;
	unsigned short _Padding() const;
	unsigned short _Ypos() const;
	Frame* _ParentFrame() const;
	FrameElement* _Original() const;

	virtual FrameElement* _Clone();
	virtual ~FrameElement();
};

inline void FrameElement::_Show() {}

inline void FrameElement::_Hide() {}

inline void FrameElement::_SetAlign(const char* align) {
	this->align = align;
}

inline bool FrameElement::_YposSet() const {
	return this->yposSet;
}

inline void FrameElement::_SetPadding(unsigned short padding) {
	this->padding = padding;
	this->paddingSet = true;
}

inline Cursor FrameElement::_Align() {
	Cursor pos;
	return pos;
}

inline void FrameElement::_SetYpos(unsigned short y) {
	this->Ypos = y;
	this->yposSet = true;
}

inline void FrameElement::_SetParentFrame(Frame* parentFrame) {
	this->parentFrame = parentFrame;
}

inline unsigned short FrameElement::_Ypos() const {
	return this->Ypos;
}

inline unsigned short FrameElement::_Padding() const {
	return this->padding;
}

inline bool FrameElement::_PaddingSet() const {
	return this->paddingSet;
}

inline Frame* FrameElement::_ParentFrame() const {
	return this->parentFrame;
}

inline FrameElement* FrameElement::_Clone() {
	FrameElement* clone = new FrameElement(*this);
	clone->original = this;
	this->clone = clone;
	return clone;
}

inline FrameElement* FrameElement::_Original() const {
	return this->original;
}

class Separator : public FrameElement {
private:
	friend void Display::_Display(Separator& separator);
	void _SetValue(Frame& parentFrame, unsigned short start_X, unsigned short start_Y);

public:
	short length;
	bool direction;

	// draw line - 0 = x spawn direction, 1 = y spawn direction
	Separator(Frame& parentFrame, short length, bool direction, unsigned short start_X, unsigned short start_Y);
	Separator(Layout& layout, short length, bool direction, unsigned short start_X, unsigned short start_Y);
	Separator(const Separator& copy);

	virtual Separator* _Clone();
};

inline Separator* Separator::_Clone() {
	Separator* clone = new Separator(*this);
	clone->original = this;
	this->clone = clone;
	return clone;
}

class Label : public FrameElement {
private:
	bool deleteText = false;

protected:
	Cursor _Align();	

public:	
	char* text = nullptr;
	short length = 0;
	unsigned char symbol = 0;
	unsigned short cut = 0;

	Label() {
		this->componentType = ComponentType::label;
	}
	Label(const Label& copy) : FrameElement(copy), text(utility::_CopyChar(copy.text)), length(copy.length) , symbol(copy.symbol), cut(copy.cut){ 
		this->componentType = ComponentType::label;
		deleteText = true;
		this->align = copy.align;
	}
	Label(const char* text) : text((char*)text) { 
		length = utility::_CharLength(text);
		this->componentType = ComponentType::label;
	}
	Label(const char* text, const char* align) : text((char*)text) { 
		length = utility::_CharLength(text);
		this->align = align;
		this->componentType = ComponentType::label;
	}
	Label(const char* text, unsigned char symbol, const char* align) : text((char*)text), symbol(symbol) {
		length = utility::_CharLength(text) + 1;
		this->align = align;
		this->componentType = ComponentType::label;
	}
	Label(int num) : length(utility::_DigitNumberInt(num)) {
		this->text = utility::_IntToChar(num);
		this->componentType = ComponentType::label;
		this->deleteText = true;
	}
	Label(Date& date) : text(date._DateChar()) {
		this->componentType = ComponentType::label;
		this->deleteText = true;
		this->length = utility::_CharLength(text);
	}	
	Label(char* text) : text(utility::_CopyChar(text)), length(utility::_CharLength(text)) {
		this->componentType = ComponentType::label;
		this->deleteText = true;
	}

	void _SetText(const char* text);

	virtual short _Length() const;
	virtual void _Show() override;
	virtual void _Hide() override;	
	virtual Label* _Clone();

	virtual ~Label() {
		if (deleteText)
			delete[]text;
	}
};

inline short Label::_Length() const {
	return this->length;
}

inline void Label::_SetText(const char* text) {
	this->text = (char*)text;
	this->length = utility::_CharLength(text);
}

inline Label* Label::_Clone() {
	Label* clone = new Label(*this);
	clone->original = this;
	this->clone = clone;
	return clone;
}

class Layout : public FrameElement {	
private:
	Frame* frame = nullptr;

public:
	Layout(Frame* parentFrame) : FrameElement(parentFrame) { this->componentType = ComponentType::layout; }
	Layout(const Layout& copy) : FrameElement(copy.parentFrame), frame(copy.frame) { this->componentType = ComponentType::layout; }
	Frame * _Select();
	Frame* _Select(const char* IDname);
	void _Split(Separator& separator, const char* firstID, const char* secondID);
	void _DefaultFrameTemplate(Display& dsp);
	void _ShowElements();
	std::vector<FrameElement*> _SelectElements(ComponentType type);

	virtual Layout* _Clone();

	~Layout();
};

inline Layout* Layout::_Clone() {
	Layout* clone = new Layout(*this);
	clone->original = this;
	this->clone = clone;
	return clone;
}

class MenuItem;
class Menu : public FrameElement {
private:
	void _AddItem(MenuItem& item);
	bool deleteItems = false;

public:
	int size = 0;
	int linkNum = 0;
	MenuItem** items = nullptr;	

	//	Add more Menu items using variadic template
	template <typename T>
	void _AddItems(T& item) {
		_AddItem(item);
	}
	template<typename T, typename ... TT>
	void _AddItems(T& item, TT& ... nextItems) {
		_AddItem(item);
		_AddItems(nextItems...);
	}
	const char* _GetLink(int selection);
	void _ChangeItem(const char* text, const char* newText, const char* newLink = nullptr);
	void _Show() override;
	void _Hide() override;
	virtual Menu* _Clone();

	Menu() {
		this->componentType = ComponentType::menu;
	};
	Menu(const Menu& copy);
	~Menu();
};

inline Menu* Menu::_Clone(){
	Menu* clone = new Menu(*this);
	clone->original = this;
	this->clone = clone;
	return clone;
}

class Module;
class MenuItem : public Label {
private:
	friend const char* Menu::_GetLink(int selection);

	const char* prefix = nullptr;
	const char* link = nullptr;

public:
	char* orderNum = nullptr;
	MenuItem(const char* text, const char* moduleName = nullptr) : Label(text), link(moduleName){
		this->componentType = ComponentType::menuItem;
	}
	MenuItem(const char* text, Module* previousModule);
	MenuItem(const MenuItem& copy) : Label(copy), prefix(copy.prefix), link(copy.link), orderNum(copy.orderNum) {
		this->componentType = ComponentType::menuItem;
	}
	void _SetOrderNumber(char* orderNum);
	void _SetSpecialPrefix(const char* prefix);
	void _SetLink(const char* moduleName);
	short _Length() const override;
	void _Show() override;
	virtual MenuItem* _Clone();

	~MenuItem() = default;
};

inline void MenuItem::_SetOrderNumber(char* orderNum) {
	if(prefix==nullptr)
		this->orderNum = orderNum;
	else {
		this->orderNum = (char*)prefix;
	}	
}

inline const char* Menu::_GetLink(int selection) {
	return items[selection-1]->link;
}

inline void MenuItem::_SetSpecialPrefix(const char* prefix) {
	this->prefix = prefix;
}

inline short MenuItem::_Length() const {
	return utility::_CharLength(prefix) + this->length;
}

inline void MenuItem::_SetLink(const char* moduleName) {
	this->link = moduleName;
}

inline MenuItem* MenuItem::_Clone() {
	MenuItem* clone = new MenuItem(*this);
	clone->original = this;
	this->clone = clone;
	return clone;
}

class TextBar : public Label {
private:
	Label** items = nullptr;
	int num = 0;
	int spacing = 1;
	bool deleteItems = false;

	void _AddItem(Label& item);

public:
	template <typename T>
	void _AddItems(T& item) {
		_AddItem(item);
	}
	template<typename T, typename ... TT>
	void _AddItems(T& item, TT& ... nextItems) {
		_AddItem(item);
		_AddItems(nextItems...);
	}

	template<typename ... TT>
	TextBar(TT& ... items) {	
		_AddItems(items ...);
		this->componentType = ComponentType::textBar;
	}

	void _SetSpacing(int spacing);
	void _Show() override;
	virtual TextBar* _Clone();

	// copy constructor without const - gives advantage over variadic template
	TextBar(TextBar& copy);
	
	~TextBar();
};

inline void TextBar::_SetSpacing(int spacing) {
	this->spacing = spacing;
}

inline TextBar* TextBar::_Clone() {
	TextBar* clone = new TextBar(*this);
	clone->original = this;
	this->clone = clone;
	return clone;
}

class Table : public FrameElement {
	int rowNum = 0;
	int colNum = 0;
	
	bool showBorder = false;
	bool cellSeparator = true;

	void _DrawBorder();
public:
	void _ToggleBorder(bool status);
	void _ToggleCellSeparator(bool status);
	void _SetColumnWidth(int columnIndex, int width);
	void _Show() override;
	virtual Table* _Clone();

	Frame*** cells = nullptr;

	Table(Frame* parentFrame, int rowNum, int colNum);
	Table(const Table& copy);
	~Table();
};

inline Table* Table::_Clone() {
	Table* clone = new Table(*this);
	clone->original = this;
	this->clone = clone;
	return clone;
}

inline void Table::_ToggleBorder(bool status) {
	this->showBorder = status;
	if (showBorder)
		this->cellSeparator = false;
}

inline void Table::_ToggleCellSeparator(bool status) {
	this->cellSeparator = status;
	if (status)
		this->showBorder = false;
}