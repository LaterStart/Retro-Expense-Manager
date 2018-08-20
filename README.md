# Retro-Expense-Manager
my final year project in progress

Idea was to build complex CMD application from scratch without help of any external libraries, for educational purposes.
Retro Expense Manager is "simulated" financial application, that helps user to track his everyday financial transactions.
It's built using MVC design pattern, while data records are stored in binary format.
Application has exchange rate feature, which connects to online exchange rate API, downloading most recent exhange rates.
Altought application is CMD app, it has simulated GUI elements, which enable user to use and browse application with his keyboard.
Modules are loaded during compile time, using factory design pattern. Each module adds itself to module registar, 
which is being loaded during run time.
More detailed documentation is yet to come. Have fun!
