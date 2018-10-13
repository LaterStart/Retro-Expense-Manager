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
	selector->_Split(unsigned short(1.0 * width / selector->_Width() * 100), "vertical", "Income", "Temp");
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
	layout._ShowElements();

	//	Latest transaction table - table instance should always be inside main module scope
	Table table(layout._Select("LatestTransactionsBody"), LT->size(), 6);
	int minID = transactionController._LastTransactionID(), maxID = 0;
	if (LT->size() > 0) {
		const int  maxAccountLength = 18, maxCategoryLength = 15;
		int highestAmountLength = 0, highestAccountLength = 0, highestCategoryLength = 0;
		for (size_t i = 0; i < LT->size(); i++) {
			Transaction* TR = &LT->at(i);
			Currency* CU = exchangeRateController._GetCurrency(TR->_Currency());
			Account* AC = accountController._GetAccount(TR->_Account());
			Account* AC_from = nullptr;
			Category* CA = categoryController._GetCategory(TR->_Category());

			table.cells[i][0]->_AddElements(IDLabel("ID", TR->_ID()));
			table.cells[i][1]->_AddElements(Label(TR->_Date()));
			table.cells[i][2]->_AddElements(TextBar(Label(TR->_AmountChar()), Label(CU->_Name())));
			if (TR->_Type() == TransactionType::transfer) {
				AC_from = accountController._GetAccount(TR->_FromAccount());				
				table.cells[i][3]->_AddElements(TransferBar(Label(AC_from->_Name()), Label(AC->_Name())));
			}
			else table.cells[i][3]->_AddElements(Label(AC->_Name()));			
			table.cells[i][4]->_AddElements(Label(CA->_Name()));
			table.cells[i][5]->_AddElements(Label(TR->_Description()));

			int amountLength = utility::_CharLength(TR->_AmountChar());
			int accountLength;
			if (TR->_Type() == TransactionType::transfer) 
				accountLength = utility::_CharLength(AC->_Name()) + utility::_CharLength(AC_from->_Name()) + 3;			
			else accountLength = utility::_CharLength(AC->_Name());			 
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

		ltFrame->_Split(36, "vertical", "EditTransaction", "ETSpec");	
		Frame* etFrame = ltFrame->_Select("EditTransaction");
		etFrame->_AddLeftPadding(2);
		etFrame->_AddTopPadding(1);
	}
	else {
		Label text1("You have no recent transactions.");
		Label text2("Select [1] to add new transaction record.");
		text2._SetYpos(2);

		ltFrame->_AddElements(text1, text2);
	}

	//	Main dashboard user input
	int selection = 0;
	bool endMainLoop = false;
	bool switchModule = false;
	while (selection <  1 || selection > mainMenu.size) {
		if (LT->size() > 0)
			controlMenu._Show();
		ltFrame->_ShowElements();		
		Cursor(2, ::height - 4);
		UserInput select(InputType::select);
		select._ReadUserInput();
		selection = select.selection;

		if (select.control == ControlKey::F1 && LT->size() > 0) {
			controlMenu._ChangeItem("Select Transaction", "Menu");
			select._ClearInput();
			int selectedID;
			while (true) {
				//	Select transaction input
				Cursor(ltFrame->_X1(), ltFrame->_Y2() - 1);
				UserInput select(InputType::integer);
				if(table._Visible() == false)
					table._Show();
				ETlabel._Show();
				int selection = -1;
				bool mainInput = false;
				while (selection <  minID || selection > maxID || select._ControlKey()) {
					Cursor pos;
					select._ReadUserInput();
					selection = select.selection;
					if (select.control == ControlKey::F1) {
						controlMenu._ChangeItem("Menu", "Select Transaction");
						select._ClearInput();
						ETlabel._Hide();
						mainInput = true;
						break;
					}
					else if (select._ControlKey())
						continue;
					select._ClearInput();
					pos._SetCursorPosition();
					selectedID = selection;
				}
				if (mainInput)
					break;
				else {
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

					Frame tdFrame(*ltFrame);
					tdFrame._AddTopPadding(1);
					Currency* cu = exchangeRateController._GetCurrency(tr->_Currency());
					Account* ac = accountController._GetAccount(tr->_Account());
					Category* ca = categoryController._GetCategory(tr->_Category());
					std::vector<const char*>* types = &transactionController.transactionType;
					int trType = static_cast<int>(tr->_Type());
					Account* from_ac = accountController._GetAccount(tr->_FromAccount());

					TextBar t1(Label("Type:"), Label(types->at(trType)));
					TextBar t2(Label("Date:"), Label(tr->_Date()));
					TextBar t3(Label("Amount:"), Label(tr->_AmountChar()));
					TextBar t4(Label("Currency:"), Label(cu->_Name()));
					TextBar* t5 = nullptr;
					TextBar* t8 = nullptr;
					if (tr->_Type() == TransactionType::transfer) {
						t5 = new TextBar(Label("From Account:"), Label(from_ac->_Name()));
						t8 = new TextBar(Label("To Account:"), Label(ac->_Name()));
					}
					else t5 = new TextBar(Label("Account:"), Label(ac->_Name()));
					TextBar t6(Label("Category:"), Label(ca->_Name()));
					TextBar t7(Label("Description:"), Label(tr->_Description()));

					if(tr->_Type() == TransactionType::transfer)
						tdFrame._AddElements(t1, t2, t3, t4, *t5, *t8 ,t6, t7);
					else tdFrame._AddElements(t1, t2, t3, t4, *t5, t6, t7);
					delete t5;
					delete t8;

					// show transaction details					
					while (true) {
						bool cancel = false;
						tdFrame._ShowElements();

						//	User input
						Cursor(ltFrame->_X1(), ltFrame->_Y2() - 1);
						UserInput select(InputType::select);
						select._ReadUserInput();
						if (select.control == ControlKey::F1) {
							F2._Hide();
							DEL._Hide();
							ESC._Hide();
							select._ClearInput();
							controlMenu._ChangeItem("Back", "Cancel");
							controlMenu._ChangeItem("Menu", "Back");
							ESC._SetPadding(F1._Length() + 1);
							ESC._Show();
							selection = 0;
							// Menu input					
							while (selection <  1 || selection > mainMenu.size) {
								Cursor(2, ::height - 4);
								UserInput select(InputType::select);
								select._ReadUserInput();
								selection = select.selection;
								if (select.control == ControlKey::F1) {
									ESC._Hide();
									controlMenu._ChangeItem("Back", "Menu");
									controlMenu._ChangeItem("Cancel", "Back");
									F2._Show();
									DEL._Show();
									ESC._SetPadding(F1._Length() + F2._Length() + DEL._Length() + 3);
									ESC._Show();
									select._ClearInput();
									break;
								}	
								else if (select.control == ControlKey::esc) {
									cancel = true;
									break;
								}
								select._ClearInput();
							}
						}
						else if (select.control == ControlKey::esc) {
							F2._Hide();
							DEL._Hide();
							ESC._Hide();
							tdFrame._HideElements();
							selectTransactionTitle._Hide();
							latestTransactionTitle._Show();
							table._Show();
							break;
						}
						else if (select.control == ControlKey::F2) {
							tdFrame._HideElements();
							controlMenu._ChangeItem("Back", "Cancel");
							F2._Hide();
							DEL._Hide();
							ESC._Hide();
							ESC._SetPadding(F1._Length() + 1);
							ESC._Show();

							vector<Container> formStore{
								Container(trType),
								Container(tr->_Date()),
								Container(utility::_DoubleToChar(tr->_Amount())),
								Container(cu->_ID()),
								Container(ac->_ID()),
								Container(ca->_ID()),
								Container(tr->_Description())
							};		

							if (tr->_Type() == TransactionType::transfer)
								formStore.insert(formStore.begin() + 4, Container(from_ac->_ID()));

							Frame etFrame(*ltFrame->_Select("EditTransaction"));
							Form editTransaction;
							editTransaction._SetParentFrame(&etFrame);
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
							editTransaction._SetSpecialContentFrame(ltFrame->_Select("ETSpec"));

							//	Transfer event - If user selects transfer as transaction type, change account field
							const function<void(Form&, FormField*)> transferEvent = [](Form& form, FormField* currentField) {
								if (form._EventStatus(0) == false) {
									FormField* cField = form._SelectField(Field::transactionType);
									if (cField->inputField->selection == 2) {
										// remove one field at position 4
										form._RemoveFields(4, 1);
										form._InsertFields(
											//	Adds two new fields at position 4 and 5
											tuple<ScrollDown<Account>&, int>{ ScrollDown<Account>("From Account:", *accountController.accounts, Field::account), 4 },
											tuple<ScrollDown<Account>&, int>{ ScrollDown<Account>("To Account:", *accountController.accounts, Field::account), 5 }
										);
										form._InitializeFields();
										form._SetEventStatus(0, true);
									}
								}
								else {
									//  If user changed his mind and wants not to select transfer as transaction type
									FormField* cField = form._SelectField(Field::transactionType);
									if (cField->inputField->selection != 2) {
										// remove two fields starting at position 4 and insert one new field at position 4
										form._RemoveFields(4, 2);
										form._InsertFields(tuple<ScrollDown<Account>&, int>{ ScrollDown<Account>("Account:", *accountController.accounts, Field::account), 4 });
										form._InitializeFields();
										form._SetEventStatus(0, false);
									}
								}
							};
							editTransaction._AddEvent(transferEvent);	

							while (true) {
								// start form 
								editTransaction._Show();
								if (editTransaction._Status()) {
									// if form is completed - update transaction
									//	Swap input selection with selected model IDs	
									vector<FormField*> fields = editTransaction._SelectFields(vector<Field>{Field::category, Field::currency, Field::account});
									ScrollDown_2D<Category>* categoryField = dynamic_cast<ScrollDown_2D<Category>*>(fields.at(0));
									ScrollDown<Currency>* currencyField = dynamic_cast<ScrollDown<Currency>*>(fields.at(1));
									ScrollDown<Account>* accountField1 = dynamic_cast<ScrollDown<Account>*>(fields.at(2));
									ScrollDown<Account>* accountField2 = nullptr;
									if (fields.size() > 3)
										accountField2 = dynamic_cast<ScrollDown<Account>*>(fields.at(3));
									Category* category = categoryField->_Value();
									Currency* currency = &currencyField->_Items().at(currencyField->inputField->selection);
									Account* account1 = &accountField1->_Items().at(accountField1->inputField->selection);
									Account* account2 = nullptr;
									if (accountField2 != nullptr)
										account2 = &accountField2->_Items().at(accountField2->inputField->selection);
									fields.at(0)->inputField->selection = category->_ID();
									fields.at(1)->inputField->selection = currency->_ID();
									fields.at(2)->inputField->selection = account1->_ID();
									if (account2 != nullptr)
										fields.at(3)->inputField->selection = account2->_ID();

									//	Get form data and pass it to controller
									utility::LinkedList<Data*>* data = editTransaction._GetData();										
									Transaction transaction = transactionController._EditTransaction(data, selectedID, profileController._ActiveProfile()->_ID());

									//	Update account balance
									double amount = transaction._Amount() - tr->_Amount();
									if (transaction._Type() == TransactionType::transfer) {
										Currency* acc1ccy = exchangeRateController._GetCurrency(account1->_DefaultCurrency());
										Currency* acc2ccy = exchangeRateController._GetCurrency(account2->_DefaultCurrency());
										bool converted = false;

										if (!account1->_MultiCurrency() && account1->_DefaultCurrency() != currency->_ID()) {
											double amountCnv = exchangeRateController._ConvertCurrency(amount, currency->_ID(), acc1ccy->_ID());
											account1->_UpdateBalance(amountCnv*-1.0);
											converted = true;
										}
										else if (account1->_MultiCurrency())
											account1->_UpdateBalance(amount*-1.0, currency->_ID());
										else account1->_UpdateBalance(amount*-1.0);
										accountController._UpdateAccount(account1);

										if (converted) {
											if (!account2->_MultiCurrency() && account1->_DefaultCurrency() != account2->_DefaultCurrency()) {
												amount = exchangeRateController._ConvertCurrency(amount, account1->_DefaultCurrency(), account2->_DefaultCurrency());
												account2->_UpdateBalance(amount);
											}
											else if (account2->_MultiCurrency())
												account2->_UpdateBalance(amount, currency->_ID());
											else account2->_UpdateBalance(amount);
										}
										else {
											if (!account2->_MultiCurrency() && currency->_ID() != account2->_DefaultCurrency()) {
												amount = exchangeRateController._ConvertCurrency(amount, currency->_ID(), account2->_DefaultCurrency());
												account2->_UpdateBalance(amount);
											}
											else if (account2->_MultiCurrency())
												account2->_UpdateBalance(amount, currency->_ID());
											else account2->_UpdateBalance(amount);
										}
										accountController._UpdateAccount(account2);
									}
									else {
										amount = (transaction._Type() == TransactionType::expense) ? amount * -1.0 : amount;
										if (!account1->_MultiCurrency()) {
											Currency* accCurrency = exchangeRateController._GetCurrency(account1->_DefaultCurrency());
											if (accCurrency->_ID() != currency->_ID())
												amount = exchangeRateController._ConvertCurrency(amount, currency->_ID(), accCurrency->_ID());
											account1->_UpdateBalance(amount);
										}
										else account1->_UpdateBalance(amount, currency->_ID());
										accountController._UpdateAccount(account1);
									}									

									//	Update latest transactions list									
									transactionController._LoadLatestTransactions();
									cancel = true;
									endMainLoop = true;
									moduler->_SetNextModule("Dashboard");
									break;
								}
								else if (editTransaction._IsPaused()) {
									// if form is paussed (f1 pressed) - menu selection
									controlMenu._ChangeItem("Menu", "Back");
									Cursor(2, ::height - 4);
									UserInput select(InputType::select);
									int selection = 0;
									const char* nextModule = nullptr;
									bool unpause = false;
									while (selection <  1 || selection > mainMenu.size) {
										select._ReadUserInput();
										selection = select.selection;
										if (select.control == ControlKey::esc) {
											if (editTransaction._Exit()) {
												select._ClearInput();
												cancel = true;
												controlMenu._ChangeItem("Back", "Menu");
												controlMenu._ChangeItem("Cancel", "Back");
												ESC._Hide();
												break;
											}
											else {
												unpause = true;
												break;
											}
										}
										else if (select.control == ControlKey::F1) {
											controlMenu._ChangeItem("Back", "Menu");
											unpause = true;
											break;
										}
										select._ClearInput();
									}
									if (cancel)
										break;
									else if (unpause)
										continue;

									nextModule = mainMenu._GetLink(selection);
								}
								else if (!editTransaction._Status()) {
									// if form status is false - form canceled (esc)
									controlMenu._ChangeItem("Cancel", "Back");
									ESC._Hide();
									cancel = true;
									break;
								}
							}
							if (cancel) 							
								break;
						}						
						if (cancel) {				
							tdFrame._HideElements();
							F2._Hide(); DEL._Hide(); ESC._Hide();
							controlMenu._ChangeItem("Back", "Menu");
							table._Show();
							break;
						}
					}
					if (mainInput || endMainLoop)
						break;
				}
			}
			select._ClearInput();
		}
		else switchModule = true;
		if (endMainLoop)
			break;
	}	
	if (switchModule && selection != -1)
		moduler->_SetNextModule(mainMenu._GetLink(selection), this);		
}