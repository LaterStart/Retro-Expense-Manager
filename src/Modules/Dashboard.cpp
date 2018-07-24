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

	vector<Transaction>* LT = transactionController._GetLatestTransactions();
	Frame* ltFrame = layout._Select("LatestTransactionsBody");
	ltFrame->_AddLeftPadding(2);
	Table table(layout._Select("LatestTransactionsBody"), LT->size(), 6);
	const int maxAmountLength = 15;
	int highestAmountLength = 0;
	for (size_t i = 0; i < LT->size(); i++) {
		Transaction* TR = &LT->at(i);
		table.cells[i][0]->_AddElements(TextBar(Label("ID"), Label(TR->_ID())));
		table.cells[i][1]->_AddElements(Label(TR->_Date()));
		table.cells[i][2]->_AddElements(TextBar(Label(TR->_AmountChar()), Label(exchangeRateController.currencies->at(TR->_Currency())._Name())));
		table.cells[i][3]->_AddElements(Label(categoryController._GetCategory(TR->_Category())->_Name()));
		table.cells[i][4]->_AddElements(Label(accountController.accounts->at(TR->_Account())._Name()));		
		table.cells[i][5]->_AddElements(Label(TR->_Description()));
		int amountLength = utility::_CharLength(TR->_AmountChar());
		if (amountLength > highestAmountLength)
			highestAmountLength = amountLength;
	}
	highestAmountLength = (highestAmountLength <= maxAmountLength) ? highestAmountLength : maxAmountLength;
	table._SetColumnWidth(0, 4-1);
	table._SetColumnWidth(1, 10+1);
	table._SetColumnWidth(2, highestAmountLength + 5);

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