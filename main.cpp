#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <iomanip>
#include <ctime>
#include <cctype>

using namespace std;

struct Account {
    int accountNumber;
    string name;
    string pin;
    double balance;
};

vector<Account> accounts;

const string ACCOUNT_FILE = "data/accounts.txt";
const string TRANSACTION_FILE = "data/transactions.txt";

void loadAccounts() {
    accounts.clear();
    ifstream file(ACCOUNT_FILE);
    string line;

    while (getline(file, line)) {
        if (line.empty()) continue;

        stringstream ss(line);
        string number, name, pin, balance;

        getline(ss, number, '|');
        getline(ss, name, '|');
        getline(ss, pin, '|');
        getline(ss, balance, '|');

        Account a;
        a.accountNumber = stoi(number);
        a.name = name;
        a.pin = pin;
        a.balance = stod(balance);

        accounts.push_back(a);
    }
}

void saveAccounts() {
    ofstream file(ACCOUNT_FILE);
    for (const auto& a : accounts) {
        file << a.accountNumber << "|"
             << a.name << "|"
             << a.pin << "|"
             << fixed << setprecision(2)
             << a.balance << "\n";
    }
}

string currentTime() {
    time_t now = time(nullptr);
    tm* local = localtime(&now);
    char buffer[30];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", local);
    return string(buffer);
}

void logTransaction(int accountNumber, const string& type, double amount) {
    ofstream file(TRANSACTION_FILE, ios::app);
    file << accountNumber << "|"
         << currentTime() << "|"
         << type << "|"
         << fixed << setprecision(2)
         << amount << "\n";
}

int findAccount(int number) {
    for (int i = 0; i < accounts.size(); i++)
        if (accounts[i].accountNumber == number)
            return i;
    return -1;
}

int login() {
    int number;
    string pin;

    cout << "\nAccount Number: ";
    cin >> number;
    cout << "PIN: ";
    cin >> pin;

    int index = findAccount(number);
    if (index == -1) {
        cout << "Account not found.\n";
        return -1;
    }

    if (accounts[index].pin != pin) {
        cout << "Incorrect PIN.\n";
        return -1;
    }

    cout << "\nLogin successful. Welcome, " << accounts[index].name << "!\n";
    return index;
}

void checkBalance(int index) {
    cout << fixed << setprecision(2);
    cout << "\nBalance: Rs. " << accounts[index].balance << "\n";
}

void withdraw(int index) {
    double amount;
    cout << "\nEnter amount: Rs. ";
    cin >> amount;

    if (amount <= 0) {
        cout << "Invalid amount.\n";
        return;
    }
    if (amount > accounts[index].balance) {
        cout << "Insufficient balance.\n";
        return;
    }

    accounts[index].balance -= amount;
    saveAccounts();
    logTransaction(accounts[index].accountNumber, "WITHDRAW", amount);
    cout << "Withdrawal successful.\n";
}

void deposit(int index) {
    double amount;
    cout << "\nEnter amount: Rs. ";
    cin >> amount;

    if (amount <= 0) {
        cout << "Invalid amount.\n";
        return;
    }

    accounts[index].balance += amount;
    saveAccounts();
    logTransaction(accounts[index].accountNumber, "DEPOSIT", amount);
    cout << "Deposit successful.\n";
}

void transfer(int index) {
    int recipientNumber;
    double amount;

    cout << "\nRecipient account: ";
    cin >> recipientNumber;

    int recipient = findAccount(recipientNumber);
    if (recipient == -1) {
        cout << "Recipient account not found.\n";
        return;
    }

    if (recipient == index) {
        cout << "Cannot transfer to the same account.\n";
        return;
    }

    cout << "Amount: Rs. ";
    cin >> amount;

    if (amount <= 0 || amount > accounts[index].balance) {
        cout << "Invalid amount or insufficient balance.\n";
        return;
    }

    accounts[index].balance -= amount;
    accounts[recipient].balance += amount;
    saveAccounts();

    logTransaction(accounts[index].accountNumber, "TRANSFER_SENT", amount);
    logTransaction(accounts[recipient].accountNumber, "TRANSFER_RECEIVED", amount);

    cout << "Transfer successful.\n";
}

void miniStatement(int accountNumber) {
    ifstream file(TRANSACTION_FILE);
    string line;
    bool found = false;

    cout << "\n========== MINI STATEMENT ==========\n";

    while (getline(file, line)) {
        stringstream ss(line);
        string account, date, type, amount;
        
        getline(ss, account, '|');
        getline(ss, date, '|');
        getline(ss, type, '|');
        getline(ss, amount, '|');

        if (stoi(account) == accountNumber) {
            cout << date << " | " << left << setw(20) << type << " | Rs. " << amount << "\n";
            found = true;
        }
    }

    if (!found) cout << "No transactions found.\n";
    cout << "====================================\n";
}

void changePin(int index) {
    string oldPin, newPin;

    cout << "\nCurrent PIN: ";
    cin >> oldPin;

    if (oldPin != accounts[index].pin) {
        cout << "Incorrect PIN.\n";
        return;
    }

    cout << "New 4-digit PIN: ";
    cin >> newPin;

    if (newPin.length() != 4) {
        cout << "PIN must contain exactly 4 digits.\n";
        return;
    }

    for (char c : newPin) {
        if (!isdigit(c)) {
            cout << "PIN must contain only digits.\n";
            return;
        }
    }

    accounts[index].pin = newPin;
    saveAccounts();
    cout << "PIN changed successfully.\n";
}

void atmMenu(int index) {
    int choice;
    while (true) {
        cout << "\n========== ATM MENU ==========\n";
        cout << "1. Check Balance\n";
        cout << "2. Withdraw\n";
        cout << "3. Deposit\n";
        cout << "4. Transfer\n";
        cout << "5. Mini Statement\n";
        cout << "6. Change PIN\n";
        cout << "7. Logout\n";
        cout << "==============================\n";
        cout << "Choice: ";
        cin >> choice;

        switch (choice) {
            case 1: checkBalance(index); break;
            case 2: withdraw(index); break;
            case 3: deposit(index); break;
            case 4: transfer(index); break;
            case 5: miniStatement(accounts[index].accountNumber); break;
            case 6: changePin(index); break;
            case 7:
                cout << "Logged out.\n";
                return;
            default:
                cout << "Invalid choice.\n";
        }
    }
}

int main() {
    loadAccounts();
    int choice;

    cout << "\n====================================\n";
    cout << "        ATM MANAGEMENT SYSTEM       \n";
    cout << "====================================\n";

    while (true) {
        cout << "\n1. Login\n";
        cout << "2. Exit\n";
        cout << "Choice: ";
        cin >> choice;

        if (choice == 1) {
            int index = login();
            if (index != -1) atmMenu(index);
        } else if (choice == 2) {
            cout << "Thank you for using the ATM.\n";
            break;
        } else {
            cout << "Invalid choice.\n";
        }
    }

    return 0;
}
