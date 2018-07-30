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

	//	Control menu
	Menu controlMenu;
	MenuItem F1("Select Transaction");
	MenuItem F2("Edit");
	MenuItem DEL("Delete");
	MenuItem ESC("Back");
	F1._SetSpecialPrefix("[F1] ");
	F2._SetSpecialPrefix("[F2] ");
	F2._SetYpos(0);
	F2._SetAutoDisplay(false);
	ESC._SetSpecialPrefix("[ESC] ");
	ESC._SetYpos(0);	
	ESC._SetAutoDisplay(false);
	DEL._SetSpecialPrefix("[DEL] ");
	DEL._SetYpos(0);
	DEL._SetAutoDisplay(false);
	controlMenu._AddItems(F1, F2, ESC, DEL);

	Label ETlabel("Enter transaction ID: ");
	ETlabel._SetYpos(11);
	ETlabel._SetPadding(-1);
	ETlabel._SetAutoDisplay(false);

	//	Split content frame
	Frame* selector = layout._Select("Content");
	Separator middleSplit(*layout._Select("Body"), selector->_X2() - selector->_X1() + 2, 0, selector->_X1() - 3, (selector->_Y2() / 2) - 1);
	Separator titleLine(*layout._Select("Body"), selector->_X2() - selector->_X1() + 2, 0, selector->_X1() - 3, (selector->_Y2() / 2) + 1);
	selector->_Split(middleSplit, "Balance", "LatestTransactions");

	//	Make 3 balance frames
	selector = layout._Select("Balance");
	int width = selector->_Width() / 3;
	selector->_Split(unsigned short(1.0 * width / selector->_Width() * 100) , "vertical", "Income", "Temp");
	selector->_Select("Temp")->_Split(unsigned short(1.0 * width / (selector->_Width() - width) * 100), "vertical", "NetWorth", "Expense");

	//	Get this month stats
	int defCCYID = profileController._ActiveProfile()->_DefaultCurrency();
	vector<Transaction> TMT = transactionController._GetTransactions(Query(Range::thisMonth));
	double tmIncome = 0, tmExpense = 0;
	for (size_t i = 0; i < TMT.size(); i++) {
		int ccyID = TMT.at(i)._Currency();
		TransactionType type = TMT.at(i)._Type();
		double amount = TMT.at(i)._Amount();	
		if (ccyID != defCCYID)
			amount = exchangeRateController._ConvertCurrency(amount, ccyID, defCCYID);
		switch (type) {
		case TransactionType::expense:
			tmExpense += amount;
			break;
		case TransactionType::income:
			tmIncome += amount;
		default:
			break;
		}
	}

	//	Calculate net worth
	double netWorth = 0.0;	
	for (size_t i = 0; i < accountController.accounts->size(); i++) {
		Account* account = &accountController.accounts->at(i);
		vector<Account::Balance>* balance = account->_Balance();
		for (size_t j = 0; j < balance->size(); j++) {
			if (balance->at(j).currencyID != defCCYID) {
				double converted = exchangeRateController._ConvertCurrency(balance->at(j).totalAmount, balance->at(j).currencyID, defCCYID);
				netWorth += converted;
			}
			else netWorth += balance->at(j).totalAmount;
		}
	}
	char ch;
	if (netWorth < 0) {
		netWorth *= -1.0;
		ch = '-';
	}
	else if (netWorth == 0)
		ch = '=';
	else ch = '+';
	Label netWrth(ch, utility::_DoubleToChar(netWorth));
	Currency* ccy = exchangeRateController._GetCurrency(defCCYID);
	Label CCY(ccy->_Name());
	
	//	Total net worth and this month expense and income display
	selector->_Select("Income")->_AddElements(Label("Income this month", "center"),
		TextBar("center", Label('+', utility::_DoubleToChar(tmIncome)), CCY));

	selector->_Select("NetWorth")->_AddElements(Label("Net Worth", "center"),
		TextBar("center", netWrth, CCY));

	selector->_Select("Expense")->_AddElements(Label("Expense this month", "center"), 
		TextBar("center", Label('-', utility::_DoubleToChar(tmExpense)), CCY));

	//	Latest transactions	
	layout._Select("LatestTransactions")->_Split(titleLine, "LatestTransactionsHeader", "LatestTransactionsBody");
	display._Display(titleLine);

	Label latestTransactionTitle("Latest Transactions ", ::headerSymbol, "left");
	Label selectTransactionTitle("Transaction Details ", ::headerSymbol, "left");
	selectTransactionTitle._SetYpos(0);
	selectTransactionTitle._SetPadding(2);
	latestTransactionTitle._SetPadding(2);
	selectTransactionTitle._SetAutoDisplay(false);
	layout._Select("LatestTransactionsHeader")->_AddElements(latestTransactionTitle, selectTransactionTitle);

	vector<Transaction>* LT = transactionController._GetLatestTransactions();
	Frame* ltFrame = layout._Select("LatestTransactionsBody");
	ltFrame->_AddLeftPadding(2);	

	//	Latest transaction table - table instance should always be inside main module scope
	Table table(layout._Select("LatestTransactionsBody"), LT->size(), 6);
	int minID = transactionController._LastTransactionID(), maxID = 0;
	if (LT->size() > 0) {		
		const int  maxAccountLength = 15, maxCategoryLength = 15;
		int highestAmountLength = 0, highestAccountLength = 0, highestCategoryLength = 0;
		for (size_t i = 0; i < LT->size(); i++) {
			Transaction* TR = &LT->at(i);
			Currency* CU = exchangeRateController._GetCurrency(TR->_Currency());
			Account* AC = accountController._GetAccount(TR->_Account());
			Category* CA = categoryController._GetCategory(TR->_Category());

			table.cells[i][0]->_AddElements(IDLabel("ID", TR->_ID()));
			table.cells[i][1]->_AddElements(Label(TR->_Date()));
			table.cells[i][2]->_AddElements(TextBar(Label(TR->_AmountChar()), Label(CU->_Name())));
			table.cells[i][3]->_AddElements(Label(AC->_Name()));
			table.cells[i][4]->_AddElements(Label(CA->_Name()));
			table.cells[i][5]->_AddElements(Label(TR->_Description()));

			int amountLength = utility::_CharLength(TR->_AmountChar());
			int accountLength = utility::_CharLength(AC->_Name());
			int categoryLength = utility::_CharLength(CA->_Name());
			int descriptionLength = utility::_CharLength(TR->_Description());

			if (amountLength > highestAmountLength)
				highestAmountLength = amountLength;
			if (accountLength > highestAccountLength)
				highestAccountLength = accountLength;
			if (categoryLength > highestCategoryLength)
				highestCategoryLength = categoryLength;
			if (TR->_ID() > maxID)
				maxID = TR->_ID();
			if (TR->_ID() < minID)
				minID = TR->_ID();
		}

		highestAccountLength = (highestAccountLength < maxAccountLength) ? highestAccountLength : maxAccountLength;
		highestCategoryLength = (highestCategoryLength < maxCategoryLength) ? highestCategoryLength : maxCategoryLength;
		int idLength = utility::_DigitNumberInt(maxID);

		table._SetColumnWidth(0, idLength + 1);
		table._SetColumnWidth(1, 10 + 1);
		table._SetColumnWidth(2, highestAmountLength + 5);
		table._SetColumnWidth(3, highestAccountLength + 1);
		table._SetColumnWidth(4, highestCategoryLength + 1);
		
		ltFrame->_AddElements(table, ETlabel);
		layout._Select("Footer")->_AddElements(controlMenu);

		ltFrame->_Split(34, "vertical", "TransactionDetails", "TDSpec");
		Frame* tdFrame = ltFrame->_Select("TransactionDetails");
		tdFrame->_AddLeftPadding(2);
		tdFrame->_AddTopPadding(1);
	}
	else {
		Label text1("You have no recent transactions.");
		Label text2("Select [1] to add new transaction record.");
		text2._SetYpos(2);
		
		ltFrame->_AddElements(text1, text2);
	}	
	layout._ShowElements();
		
MainInput: 	
	//	Main dashboard user input
	Cursor(2, ::height - 4);
	UserInput select(InputType::select);
	int selection = 0;
	bool selectTransaction = false;
	while (selection <  1 || selection > mainMenu.size) {
		select._ReadUserInput();
		selection = select.selection;		
		if (select.control == ControlKey::F1 && LT->size() > 0) {	
			controlMenu._ChangeItem("Select Transaction", "Menu");
			selectTransaction = true;
			select._ClearInput();
			break;
		}
		select._ClearInput();
	}
	//	Select transaction input
	if (selectTransaction) {
SelectTransaction:
		Cursor(ltFrame->_X1(), ltFrame->_Y2() - 1);
		UserInput select(InputType::integer);
		ETlabel._Show();
		int selection = -1;
		while (selection <  minID || selection > maxID) {
			Cursor pos;
			select._ReadUserInput();
			selection = select.selection;
			if (select.control == ControlKey::F1) {
				controlMenu._ChangeItem("Menu", "Select Transaction");
				select._ClearInput();
				ETlabel._Hide();
				goto MainInput;
			}
			select._ClearInput();
			pos._SetCursorPosition();
		}

		//	Display selected transaction details
		latestTransactionTitle._Hide();
		table._Hide();
		ETlabel._Hide();
		selectTransactionTitle._Show();

		F2._SetPadding(F1._Length() + 1);
		DEL._SetPadding(F1._Length() + 1 + F2._Length() + 1);
		ESC._SetPadding(F1._Length() + 1 + F2._Length() + 1 + DEL._Length() + 1);
		F2._Show();	DEL._Show(); ESC._Show();

		Transaction* tr = nullptr;
		for (size_t i = 0; i < LT->size(); i++) {
			if (LT->at(i)._ID() == selection)
				tr = &LT->at(i);
		}

		Frame tdFrame(*ltFrame->_Select("TransactionDetails"));
		Currency* cu = exchangeRateController._GetCurrency(tr->_Currency());
		Account* ac = accountController._GetAccount(tr->_Account());
		Category* ca = categoryController._GetCategory(tr->_Category());
		std::vector<const char*>* types = &transactionController.transactionType;
		int trType = static_cast<int>(tr->_Type());

		TextBar t1(Label("Type:"), Label(types->at(trType)));
		TextBar t2(Label("Date:"), Label(tr->_Date()));
		TextBar t3(Label("Amount:"), Label(tr->_AmountChar()));
		TextBar t4(Label("Currency:"), Label(cu->_Name()));
		TextBar t5(Label("Account:"), Label(ac->_Name()));
		TextBar t6(Label("Category:"), Label(ca->_Name()));
		TextBar t7(Label("Description:"), Label(tr->_Description()));

		tdFrame._AddElements(t1, t2, t3, t4, t5, t6, t7);
		tdFrame._ShowElements();
		
		{	// User input
		ViewTransaction:
			Cursor(ltFrame->_X1(), ltFrame->_Y2() - 1);
			UserInput select(InputType::select);		
			while (true) {
				select._ReadUserInput();
				if (select.control == ControlKey::F1) {					
					select._ClearInput();
					controlMenu._ChangeItem("Back", "Cancel");
					controlMenu._ChangeItem("Menu", "Back");
					goto MenuInput;
				}
				else if (select.control == ControlKey::esc) {
					F2._Hide();
					DEL._Hide();
					ESC._Hide();
					tdFrame._HideElements();
					selectTransactionTitle._Hide();
					latestTransactionTitle._Show();
					table._Show();
					goto SelectTransaction;
				}
				else if (select.control == ControlKey::F2) {
					vector<Container> formStore{
						Container(trType),
						Container(tr->_Date()),
						Container(utility::_DoubleToChar(tr->_Amount())),
						Container(cu->_ID()),
						Container(ac->_ID()),
						Container(ca->_ID()),
						Container(tr->_Description())
					};
					Frame teFrame(*ltFrame->_Select("TransactionDetails"));
					Form editTransaction;
					editTransaction._SetParentFrame(&tdFrame);
					editTransaction._AddFields(
						SelectionField("Type:", transactionController.transactionType, Field::transactionType),
						DateField("Date:", Field::date),
						FormField("Amount:", InputType::value, Field::amount),
						ScrollDown<Currency>("Currency:", *exchangeRateController.currencies, Field::currency),
						ScrollDown<Account>("Account:", *accountController.accounts, Field::account),
						ScrollDown_2D<Category>("Category:", *categoryController.categoryList, Field::category),											
						FormField("Description:", InputType::text, Field::description),
						ConfirmField("Save?:")
					);
					editTransaction._LoadStore(formStore);
					editTransaction._SetSpecialContentPosition(Form::ContentPos::right);
					editTransaction._SetSpecialContentFrame(ltFrame->_Select("TDSpec"));
					editTransaction._Show();


				}
				select._ClearInput();
			}
		}	
		MenuInput:
		{
			Cursor(2, ::height - 4);
			UserInput select(InputType::select);
			int selection = 0;
			while (selection <  1 || selection > mainMenu.size) {
				select._ReadUserInput();
				selection = select.selection;
				if (select.control == ControlKey::F1) {
					select._ClearInput();
					controlMenu._ChangeItem("Back", "Menu");
					controlMenu._ChangeItem("Cancel", "Back");
					goto ViewTransaction;	
				}
				select._ClearInput();
			}
		}
	}

	moduler->_SetNextModule(mainMenu._GetLink(selection), this);
}