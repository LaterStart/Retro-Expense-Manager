#include "Dashboard.h"
#include "../IO/IOComponent.h"
#include "../IO/Input.h"
using namespace std;

Module& Dashboard::_GetInstance() {
	return _LoadModule();
}

Dashboard& Dashboard::_LoadModule() {
	static Dashboard module;
	return module;
}

void Dashboard::_StartModule() {
	//	Create default frame layout
	Frame* mainFrame = console->_GetMainFrame();
	Layout layout(mainFrame);
	Display display;

	layout._DefaultFrameTemplate(display);
	layout._Select("MenuHeader")->_AddElements(Label("Main Menu ", ::headerSymbol, "center"));
	layout._Select("Date")->_AddElements(Label(utility::_GetCurrentDate(), "left"));

	Label title("Dashboard ", ::headerSymbol, "left");
	layout._Select("SelectionTitle")->_AddElements(title);

	//	Main menu
	Menu mainMenu;
	mainMenu._AddItems(
		MenuItem("Add Transaction", "AddTransaction"),
		MenuItem("Add Account", "AddAccount"),
		MenuItem("Add Category", "AddCategory")
	);
	layout._Select("Menu")->_AddElements(mainMenu);

	//	Split content frame
	Frame* content = layout._Select("Content");
	Separator middleSplit(*layout._Select("Body"), content->_X2() - content->_X1() + 2, 0, content->_X1() - 3, (content->_Y2() / 2) - 1);
	Separator titleLine(*layout._Select("Body"), content->_X2() - content->_X1() + 2, 0, content->_X1() - 3, (content->_Y2() / 2) + 1);
	content->_Split(middleSplit, "NetWorth", "LatestTransactions");

	//	Latest transactions	
	layout._Select("LatestTransactions")->_Split(titleLine, "LatestTransactionsHeader", "LatestTransactionsBody");
	display._Display(titleLine);

	Label latestTransactionTitle("Latest Transactions ", ::headerSymbol, "left");
	latestTransactionTitle._SetPadding(2);
	layout._Select("LatestTransactionsHeader")->_AddElements(latestTransactionTitle);

	vector<Transaction>* latestTransactions = transactionController._GetLatestTransactions();
	Frame* ltFrame = layout._Select("LatestTransactionsBody");
	ltFrame->_AddLeftPadding(2);
	Table table(layout._Select("LatestTransactionsBody"), latestTransactions->size(), 6);
	for (int i = 0; i < latestTransactions->size(); i++) {
		table.cells[i][0]->_AddElements(Label(latestTransactions->at(i)._ID()));
		table.cells[i][1]->_AddElements(Label(latestTransactions->at(i)._Date()));
	}
	table._Show();
	layout._ShowElements();

	//	User input
	Cursor(2, ::height - 4);
	UserInput select(InputType::select);
	int selection = 0;
	while (selection <  1 || selection > mainMenu.size) {
		select._ReadUserInput();
		selection = select.selection;
		select._ClearInput();
	}
	moduler->_SetNextModule(mainMenu._GetLink(selection), this);
}