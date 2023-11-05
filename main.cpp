#include <iostream>
#include <fstream>
#include <cstring>
#include <ctime>
#include <cstdlib>
#include <algorithm>
#include <string.h>
#include <sstream>
#include <conio.h>
#define MAX 5

using namespace std;

typedef struct ATM {
    string name;
    char pin[7];
    int accountNo;
    int balance;
    int last;
    string birthday;
    string contactNumber;
} list;

list l;
list accountInfo[MAX];

// GLOBAL VARIABLES
string regName;
string regBirthday;
string regContactNo;
char regPin[7];
char confirmPin[7];
char inputPin[7];
char pinFromCard[7];
string checkDrives[2] = { "D:\\checkFd.dbf", "G:\\checkFd.dbf" };
string drives[2] = { "D:\\bank.dbf", "G:\\bank.dbf" };
int regAccountNo, regBalance, ctr = -1, exist = 0, checkName = 0, checkPin = 0, pinCharacter = 0;
int accountNoFromCard, accountIndex, otherAccIndex;

void makenull();
int insertCard();
void retrieve();
int checkFd();
void registration();
int isEmpty();
int isFull();
void insert();
int locate(int x);
void save();
void saveToCard();
void checkPinFromCard();
string enterPIN();
bool transaction(char choice);
int confirmTransaction();
void transactionDisplay();
bool transactionMenu();
string encryptPin();
bool isValidBirthdayFormat(const string& birthday);
bool isNumeric(const string& str);

int main() {
    srand(time(0));
    int i;
    system("color 06");
    makenull();
    if (insertCard() == 0) { // Card is inserted and registered
        _getch();
        system("cls");
        retrieve();

        do {
            accountIndex = locate(1);
            enterPIN();
        } while (transactionMenu());
    } else { // Card is not registered
        retrieve();
        registration();
        // After registration, go directly to entering PIN
        accountIndex = l.last; // Set accountIndex to the newly registered account
        enterPIN();
        while (transactionMenu());
    }

    remove(checkDrives[ctr].c_str()); // Corrected the 'remove' function
    cout << "EXIT" << endl;
    _getch();
    return 0;
}

void makenull() {
    l.last = -1;
}

int insertCard() {
    ofstream fp;
    cout << "I N S E R T  C A R D" << endl;

    do {
        ctr = (ctr + 1) % 2;
        fp.open(checkDrives[ctr]);
        if (fp.is_open()) {
            fp.close();
            break;
        }
    } while (true);

    ifstream checkFile(drives[ctr]);
    if (!checkFile.is_open()) {
        cout << "N O T  R E G I S T E R E D." << endl;
        cin.get(); // Wait for user input
        cout << "Press any key to continue...";
        return 1;
    } else {
        cout << "R E G I S T E R E D" << endl;
        return 0;
    }
}

int checkFd() {
    ifstream fp(checkDrives[ctr]);
    if (!fp.is_open()) {
        return 1;
    }
    return 0;
}

void retrieve() {
    ifstream fp("accDb.dbf");
    if (!fp) {
        cout << "\n\nFILL UP REGISTRATION\n" << endl;
    } else {
        while (!fp.eof()) {
            l.last++;
            fp >> accountInfo[l.last].accountNo >> accountInfo[l.last].name >> accountInfo[l.last].balance >> accountInfo[l.last].pin;
            fp.ignore();
        }
    }

    fp.close();
}

void registration() {
    int end;
    do {
        end = 1;
        l.last++;
        checkName = 0;

        cout << "Enter Name: ";
        cin.ignore(); // Ignore the newline character left in the buffer by the previous input
        getline(cin, regName);

        do {
            regAccountNo = 1000 + (rand() % 9000);
        } while (locate(1));

        cout << "Account No.: " << regAccountNo << endl;

        do {
            cout << "Enter Birthday (MM/DD/YYYY): ";
            getline(cin, regBirthday);

            if (!isValidBirthdayFormat(regBirthday)) {
                cout << "Invalid birthday format. Please use MM/DD/YYYY." << endl;
            } else {
                accountInfo[l.last].birthday = regBirthday;
                break;
            }
        } while (true);

        do {
            cout << "Contact Number (11 digits): ";
            cin >> regContactNo;

            if (cin.fail() || regContactNo.length() != 11) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Clear the input buffer
                cout << "Invalid contact number. Please enter exactly 11 numeric digits." << endl;
            } else {
                accountInfo[l.last].contactNumber = regContactNo;
                break;
            }
        } while (true);

        do {
            cout << "Initial Deposit: Php ";
            cin >> regBalance;

            if (regBalance < 5000)
                cout << "Initial Deposit must be 5000 +" << endl;
            else {
                accountInfo[l.last].balance = regBalance; // Save initial deposit to the balance
                break;
            }
        } while (true);

        do {
        checkPin = 0;
        pinCharacter = 0;

        cout << "Enter PIN (6 digits): ";
        strcpy(regPin, encryptPin().c_str());
        // Check if the entered PIN has exactly 6 digits
        if (strlen(regPin) != 6 || !all_of(regPin, regPin + 6, ::isdigit)) {
            cout << "\nInvalid PIN format. Please enter exactly 6 numeric digits." << endl;
            continue; // Restart the loop
        }
        regPin[6] = '\0'; // Null-terminate the string
        cout << endl;

        cout << "Confirm PIN (6 digits): ";
        strcpy(confirmPin, encryptPin().c_str());
        // Check if the confirmed PIN has exactly 6 digits
        if (strlen(confirmPin) != 6 || !all_of(confirmPin, confirmPin + 6, ::isdigit)) {
            cout << "\nInvalid PIN format. Please enter exactly 6 numeric digits for confirmation." << endl;
            continue; // Restart the loop
        }
        confirmPin[6] = '\0'; // Null-terminate the string
        cout << endl;

        end = strcmp(regPin, confirmPin);

        if (end != 0) {
            cout << "P I N   D I D   N O T   M A T C H !" << endl;
        }
    } while (end != 0 || checkPin || pinCharacter);


        if (locate(0) && checkName == 1) {
            l.last--;
            cout << "A C C O U N T   E X I S T   A L R E A D Y" << endl;
            system("cls");
        }
    } while (checkName);

    if (isFull()) {
        cout << "D A T A B A S E   I S   F U L L" << endl;
    } else {
        insert();
    }

}

int isEmpty() {
    return (l.last == 0);
}

int isFull() {
    return (l.last == MAX);
}

void insert() {
    if (checkFd()) {
        cout << "F L A S H   D R I V E   E R R O R" << endl;
        cout << "Press any key to continue...";
        cin.get();
        exit(0);
    }
    accountInfo[l.last].name = regName;
    accountInfo[l.last].accountNo = regAccountNo;
    accountInfo[l.last].balance = regBalance;
    strcpy(accountInfo[l.last].pin, regPin);
    cout << "R E G I S T E R E D !!!" << endl;
    save();
    saveToCard();
}

int locate(int x) {
    if (x == 0) {
        for (int i = 0; i <= l.last; i++) {
            if (accountInfo[i].name == regName || strcmp(accountInfo[i].pin, regPin) == 0) {
                if (accountInfo[i].name == regName)
                    checkName = 1;
                if (strcmp(accountInfo[i].pin, regPin) == 0)
                    checkPin = 1;
                return 1;
            }
        }
        return 0;
    } else if (x == 1) {
        for (int i = 0; i <= l.last; i++) {
            if (accountInfo[i].accountNo == regAccountNo) {
                return 1;
            }
        }
        return 0;
    } else if (x == 2) {
        for (int i = 0; i < l.last + 1; i++) {
            if (accountInfo[i].accountNo == regAccountNo) {
                otherAccIndex = i;
                return 1;
            }
        }
        return 0;
    }
    return 0;
}

void save() {
    ofstream fp("accDb.dbf");
    for (int i = 0; i <= l.last; i++) {
        fp << accountInfo[i].accountNo << "\t" << accountInfo[i].name << "\t" << accountInfo[i].balance << "\t" << accountInfo[i].pin;
        if (i != l.last)
            fp << endl;
    }
    fp.close();
}

void saveToCard() {
    ofstream fp(drives[ctr]);
    fp << regAccountNo << "\t" << regPin;
    fp.close();
}
string encryptPin() {
    string input;
    char c;
    while (true) {
        c = getch();
        if (c == 13)  // Enter key
            break;
        cout << '*';
        input += c;
    }
    return input;
}

string enterPIN() {
    string enteredPin;
    char c;

    cout << "Enter PIN: ";
    for (int i = 0; i < 6; i++) {
        c = _getch();
        if (isdigit(c)) {
            cout << '*';
            enteredPin += c;
        } else {
            cout << "Invalid character. Please enter a numeric digit." << endl;
            // Clear the invalid input, but keep the loop index at -1 to repeat this iteration.
            enteredPin.clear();
            i = -1;
        }
    }
    cout << endl;

    return enteredPin;
}
// Function to validate the birthday format (MM/DD/YYYY)
bool isValidBirthdayFormat(const string& birthday) {
    if (birthday.length() != 10)
        return false;

    if (birthday[2] != '/' || birthday[5] != '/')
        return false;

    // Check if MM, DD, and YYYY parts are valid numbers
    string mm = birthday.substr(0, 2);
    string dd = birthday.substr(3, 2);
    string yyyy = birthday.substr(6, 4);

    if (!isNumeric(mm) || !isNumeric(dd) || !isNumeric(yyyy))
        return false;

    int mmInt = stoi(mm);
    int ddInt = stoi(dd);
    int yyyyInt = stoi(yyyy);

    if (mmInt < 1 || mmInt > 12)
        return false;

    if (ddInt < 1 || ddInt > 31)
        return false;

    // Check for specific date validations based on the month (e.g., February should not have more than 29 days)
    if (mmInt == 2) {
        // Check for leap year
        if ((yyyyInt % 4 == 0 && yyyyInt % 100 != 0) || (yyyyInt % 400 == 0)) {
            if (ddInt < 1 || ddInt > 29)
                return false;
        } else {
            if (ddInt < 1 || ddInt > 28)
                return false;
        }
    } else if ((mmInt == 4 || mmInt == 6 || mmInt == 9 || mmInt == 11) && (ddInt > 30)) {
        return false; // April, June, September, and November should not have more than 30 days
    }

    return true;
}

// Function to check if a string consists of numeric characters
bool isNumeric(const string& str) {
    return all_of(str.begin(), str.end(), ::isdigit);
}

void checkPinFromCard() {
    ifstream fp(drives[ctr]);
    if (fp.is_open()) {
        while (fp >> accountNoFromCard >> pinFromCard) {
        }
        fp.close();
    }
}

bool transaction(char choice) {
    int end;

    if (choice == 'a' || choice == 'A') {
        if (checkFd()) {
            exit(0);
        }
        cout << "Your balance:" << endl;
        cout << "Balance: Php " << accountInfo[accountIndex].balance << endl;
    } else if (choice == 'b' || choice == 'B') {
        if (accountInfo[accountIndex].balance != 0) {
            do {
                cout << "Enter amount to withdraw: Php ";
                cin >> regBalance;

                if (regBalance > accountInfo[accountIndex].balance) {
                    cout << "Amount greater than balance. Please try again." << endl;
                } else if (regBalance < 0) {
                    cout << "Negative entry. Please enter a positive amount." << endl;
                } else if (regBalance == 0) {
                    cout << "Invalid entry. Please enter a valid amount." << endl;
                }
            } while (regBalance > accountInfo[accountIndex].balance || regBalance < 0 || regBalance == 0);

            if (confirmTransaction()) {
                if (checkFd()) {
                    exit(0);
                }
                accountInfo[accountIndex].balance -= regBalance;
                system("color 02");
                cout << "Success!" << endl;
            } else {
                cout << "Cancelled" << endl;
            }
        } else {
            cout << "No balance." << endl;
        }
    } else if (choice == 'c' || choice == 'C') {
        do {
            cout << "Enter amount to deposit: Php ";
            cin >> regBalance;

            if (regBalance < 0) {
                cout << "Negative entry. Please enter a positive amount." << endl;
            } else if (regBalance == 0) {
                cout << "Invalid entry. Please enter a valid amount." << endl;
            }
        } while (regBalance < 0 || regBalance == 0);

        if (confirmTransaction()) {
            if (checkFd()) {
                exit(0);
            }
            system("color 02");
            accountInfo[accountIndex].balance += regBalance;
            cout << "Success!" << endl;
        } else {
            cout << "Cancelled" << endl;
        }
    } else if (choice == 'd' || choice == 'D') {
        if (l.last > 0) {
            if (accountInfo[accountIndex].balance != 0) {
                do {
                    otherAccIndex = -1;
                    cout << "Fund Transfer" << endl;
                    cout << "Enter Account No. you want to Transfer: ";
                    cin >> regAccountNo;

                    if (locate(2)) {
                        cout << "Account number doesn't exist." << endl;
                    } else if (accountInfo[accountIndex].accountNo == accountInfo[otherAccIndex].accountNo) {
                        otherAccIndex = -1;
                        cout << "Invalid account number." << endl;
                    } else {
                        do {
                            cout << "Enter amount to transfer: Php ";
                            cin >> regBalance;

                            if (regBalance > accountInfo[accountIndex].balance) {
                                cout << "Amount greater than balance. Please try again." << endl;
                            } else if (regBalance < 0) {
                                cout << "Negative entry. Please enter a positive amount." << endl;
                            } else if (regBalance == 0) {
                                cout << "Invalid entry. Please enter a valid amount." << endl;
                            }
                        } while (regBalance < 0 || regBalance > accountInfo[accountIndex].balance || regBalance == 0);
                    }
                } while (otherAccIndex == -1);

                if (confirmTransaction()) {
                    if (checkFd()) {
                        exit(0);
                    }
                    system("color 02");
                    accountInfo[accountIndex].balance -= regBalance;
                    accountInfo[otherAccIndex].balance += regBalance;
                    cout << "Success!" << endl;
                } else {
                    cout << "Cancelled" << endl;
                }
            } else {
                cout << "No balance." << endl;
            }
        } else if (l.last == 0) {
            cout << "No other account to transfer to." << endl;
        }
    } else if (choice == 'e' || choice == 'E') {
        string enteredPin;
        do {
            if (checkFd()) {
                exit(0);
            }
            checkPinFromCard();
            do {
                cout << "Change PIN" << endl;

                enteredPin = enterPIN();

                if (enteredPin != pinFromCard) {
                    cout << "Invalid PIN" << endl;
                }
            } while (enteredPin != pinFromCard);

            cout << "\nEnter new PIN: " << endl;
            for (int i = 0; i < 6; i++) {
                regPin[i] = _getch();
                putchar('*');
            }
            regPin[6] = '\0';

            cout << "\nConfirm new PIN: " << endl;
            for (int i = 0; i < 6; i++) {
                confirmPin[i] = _getch();
                putchar('*');
            }
            confirmPin[6] = '\0';

            end = strcmp(confirmPin, regPin);

            checkPin = 0;

            for (int i = 0; i < 6; i++) {
                if (!isdigit(confirmPin[i])) { // Use !isdigit() to check for non-numeric characters
                    checkPin = 1;
                    break;
                }
            }

            if (end != 0) {
                cout << "PIN did not match!" << endl;
            } else if (locate(0) && checkPin == 1) {
                cout << "PIN not unique" << endl;
            }
        } while (end != 0 || checkPin);

        if (checkFd()) {
            exit(0);
        }

        system("color 02");
        strcpy(accountInfo[accountIndex].pin, regPin);
        cout << "Success!" << endl;
        save();
        saveToCard();

    } else if (choice == 'x' || choice == 'X') {
        cout << "Thank you for using!" << endl;
        return 0;
    } else {
        cout << "Invalid choice" << endl;
    }

    save();
    return true;
}


int confirmTransaction() {
    char input;
    cout << "CONFIRM TRANSACTION?" << endl;
    cout << "[Y] YES       [N] NO" << endl;

    do {
        input = _getch();
        if (input == 'Y' || input == 'y') {
            return 1;
        } else if (input == 'N' || input == 'n') {
            return 0;
        }
    } while (input != 'Y' && input != 'y' && input != 'N' && input != 'n');

    return 0;
}

void transactionDisplay() {
    system("cls");
    cout << "T R A N S A C T I O N" << endl;
    cout << "Full Name: " << accountInfo[accountIndex].name << endl;
    cout << "No.: " << accountInfo[accountIndex].accountNo << endl;
    cout << "-------------------------------------------------------------------------" << endl;
    cout << " [A] Balance Inquiry       [D] Fund Transfer (Enrolled Accounts only)" << endl;
    cout << " [B] Withdraw              [E] Change PIN" << endl;
    cout << " [C] Deposit               [X] Exit" << endl;
    cout << "-------------------------------------------------------------------------" << endl;
}

bool transactionMenu() {
    do {
        char choice;
        system("cls");
        system("color 06");
        transactionDisplay();
        cout << "Option: ";
        cin >> choice;
        cin.ignore(); // Consume the newline character
        if (choice == 'X' || choice == 'x') {
            exit(0); // Exit the program
        }
        if (transaction(choice)) {
            save();
        } else {
            cout << "Transaction cancelled." << endl;
        }
        cout << "Press Enter to continue...";
        cin.get();
    } while (true);
}
