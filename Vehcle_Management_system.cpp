#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <string>
#include <sstream>
#include <limits>
using namespace std;

// Forward declarations
class Admin;
class Renter;

// Ensure a file exists or create it if missing
void ensureFileExists(const string& filename) {
    ifstream file(filename);
    if (!file) { // File does not exist
        ofstream newFile(filename);
        if (newFile) {
            cout << "Created file: " << filename << endl;
        }
        else {
            cerr << "Failed to create file: " << filename << endl;
        }
    }
}

// Class for item
class Item {
    string name, description;
    double rentalPrice;
    bool isAvailable;

public:
    Item(string name, string description, double rentalPrice, bool isAvailable)
        : name(name), description(description), rentalPrice(rentalPrice), isAvailable(isAvailable) {}

    string getName() { return name; }
    double getPrice() { return rentalPrice; }
    bool checkAvailability() { return isAvailable; }

    void markUnavailable() { isAvailable = false; }
    void markAvailable() { isAvailable = true; }

    void displayItem() {
        cout << "Name: " << name << ", Description: " << description
            << ", Price: $" << rentalPrice << ", Available: " << (isAvailable ? "Yes" : "No") << endl;
    }

    string toString() {
        return name + "," + description + "," + to_string(rentalPrice) + "," + (isAvailable ? "1" : "0");
    }

    static Item fromString(string data) {
        stringstream ss(data);
        string name, description, rentalPriceStr, availabilityStr;
        getline(ss, name, ',');
        getline(ss, description, ',');
        getline(ss, rentalPriceStr, ',');
        getline(ss, availabilityStr, ',');
        double rentalPrice = stod(rentalPriceStr);
        bool isAvailable = (availabilityStr == "1");
        return Item(name, description, rentalPrice, isAvailable);
    }
};

// Base Class: User
class User {
protected:
    string name, email, password, role;

public:
    User(string name, string email, string password, string role)
        : name(name), email(email), password(password), role(role) {}

    string getEmail() { return email; }
    string getPassword() { return password; }
    string getRole() { return role; }

    virtual void displayMenu(class RentalManager& rentalManager) = 0; // Pure virtual function

    virtual string toString() {
        return name + "," + email + "," + password + "," + role;
    }

    static User* fromString(string data);
};

// Derived Class: Admin
class Admin : public User {
public:
    Admin(string name, string email, string password)
        : User(name, email, password, "Admin") {}

    void displayMenu(class RentalManager& rentalManager) override;
};

// Derived Class Renter
class Renter : public User {
public:
    Renter(string name, string email, string password)
        : User(name, email, password, "Renter") {}

    void displayMenu(class RentalManager& rentalManager) override;
};

User* User::fromString(string data) {
    stringstream ss(data);
    string name, email, password, role;
    getline(ss, name, ',');
    getline(ss, email, ',');
    getline(ss, password, ',');
    getline(ss, role, ',');

    if (role == "Admin") {
        return new Admin(name, email, password);
    }
    else if (role == "Renter") {
        return new Renter(name, email, password);
    }
    return nullptr;
}

// RentalManager Class
class RentalManager {
    vector<Item> items;
    map<string, vector<string>> rentalHistory;

public:
    void loadItems() {
        ifstream file("items.txt");
        string line;
        while (getline(file, line)) {
            items.push_back(Item::fromString(line));
        }
        file.close();
    }

    void saveItems() {
        ofstream file("items.txt");
        for (auto& item : items) {
            file << item.toString() << endl;
        }
        file.close();
    }

    void loadRentalHistory() {
        ifstream file("rental_history.txt");
        string line;
        while (getline(file, line)) {
            stringstream ss(line);
            string email, item;
            getline(ss, email, ':');
            while (getline(ss, item, ',')) {
                rentalHistory[email].push_back(item);
            }
        }
        file.close();
    }

    void saveRentalHistory() {
        ofstream file("rental_history.txt");
        for (auto& entry : rentalHistory) {
            file << entry.first << ":";
            for (auto& item : entry.second) {
                file << item << ",";
            }
            file << endl;
        }
        file.close();
    }

    void addItem(Item item) {
        items.push_back(item);
    }

    void browseItems() {
        cout << "\nAvailable Items:\n";
        for (auto& item : items) {
            if (item.checkAvailability()) {
                item.displayItem();
            }
        }
    }

    void viewAllItems() {
        cout << "\nAll Items in Inventory:\n";
        for (auto& item : items) {
            item.displayItem();
        }
    }

    void reserveItem(string userEmail, string itemName) {
        for (auto& item : items) {
            if (item.getName() == itemName && item.checkAvailability()) {
                item.markUnavailable();
                rentalHistory[userEmail].push_back(itemName);
                cout << "Item reserved successfully!\n";
                saveItems();
                saveRentalHistory();
                return;
            }
        }
        cout << "Item not available or does not exist.\n";
    }

    void viewRentalHistory(string userEmail) {
        cout << "\nRental History for " << userEmail << ":\n";
        for (auto& item : rentalHistory[userEmail]) {
            cout << item << endl;
        }
    }
};

// UserManager Class
class UserManager {
    vector<User*> users;

public:
    void loadUsers() {
        ifstream file("users.txt");
        string line;
        while (getline(file, line)) {
            users.push_back(User::fromString(line));
        }
        file.close();
    }

    void saveUsers() {
        ofstream file("users.txt");
        for (auto& user : users) {
            file << user->toString() << endl;
        }
        file.close();
    }

    User* authenticate(string email, string password) {
        for (auto& user : users) {
            if (user->getEmail() == email && user->getPassword() == password) {
                return user;
            }
        }
        return nullptr;
    }

    void addUser(User* user) {
        users.push_back(user);
        saveUsers();
    }

    void registerUser() {
        string name, email, password, role;
        cout << "Enter Name: ";
        cin.ignore();
        getline(cin, name);
        cout << "Enter Email: ";
        getline(cin, email);
        cout << "Enter Password: ";
        getline(cin, password);
        cout << "Role (Admin/Renter): ";
        getline(cin, role);

        if (role == "Admin") {
            addUser(new Admin(name, email, password));
        }
        else if (role == "Renter") {
            addUser(new Renter(name, email, password));
        }
        else {
            cout << "Invalid role. Registration failed.\n";
        }
    }

    void displayUsers() {
        cout << "\nRegistered Users:\n";
        for (size_t i = 0; i < users.size(); ++i) {
            cout << i + 1 << ". " << users[i]->toString() << endl;
        }
    }

    void editUser() {
        displayUsers();
        cout << "Enter the number of the user to edit: ";
        int index;
        cin >> index;
        if (index > 0 && index <= users.size()) {
            string name, email, password, role;
            cout << "Enter New Name: ";
            cin.ignore();
            getline(cin, name);
            cout << "Enter New Email: ";
            getline(cin, email);
            cout << "Enter New Password: ";
            getline(cin, password);
            cout << "Enter New Role (Admin/Renter): ";
            getline(cin, role);

            delete users[index - 1];
            if (role == "Admin") {
                users[index - 1] = new Admin(name, email, password);
            }
            else if (role == "Renter") {
                users[index - 1] = new Renter(name, email, password);
            }
            else {
                cout << "Invalid role. Edit failed.\n";
                return;
            }
            saveUsers();
            cout << "User updated successfully.\n";
        }
        else {
            cout << "Invalid user number.\n";
        }
    }

    void deleteUser() {
        displayUsers();
        cout << "Enter the number of the user to delete: ";
        int index;
        cin >> index;
        if (index > 0 && index <= users.size()) {
            delete users[index - 1];
            users.erase(users.begin() + index - 1);
            saveUsers();
            cout << "User deleted successfully.\n";
        }
        else {
            cout << "Invalid user number.\n";
        }
    }
};

// Implementation of Admin::displayMenu
void Admin::displayMenu(RentalManager& rentalManager) {
    UserManager userManager;
    userManager.loadUsers();

    int choice;
    while (true) {
        cout << "\nAdmin Menu:\n";
        cout << "1. Add Item\n";
        cout << "2. View All Items\n";
        cout << "3. Manage Users\n";
        cout << "4. Exit to Main Menu\n";
        cout << "Choose an option: ";
        cin >> choice;

        switch (choice) {
        case 1: {
            string name, description;
            double price;
            char addAnother;
            do {
                cout << "Enter item name: ";
                cin.ignore();
                getline(cin, name);
                cout << "Enter item description: ";
                getline(cin, description);
                cout << "Enter rental price: ";
                while (!(cin >> price)) {
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    cout << "Invalid price. Enter a numeric value: ";
                }
                rentalManager.addItem(Item(name, description, price, true));
                rentalManager.saveItems();
                cout << "Item added successfully.\n";
                cout << "Do you want to add another item? (y/n): ";
                cin >> addAnother;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
            } while (tolower(addAnother) == 'y');
            break;
        }
        case 2:
            rentalManager.viewAllItems();
            break;
        case 3: {
            int manageChoice;
            while (true) {
                cout << "\nManage Users Menu:\n";
                cout << "1. View Users\n";
                cout << "2. Add User\n";
                cout << "3. Edit User\n";
                cout << "4. Delete User\n";
                cout << "5. Back to Admin Menu\n";
                cout << "Choose an option: ";
                cin >> manageChoice;

                switch (manageChoice) {
                case 1:
                    userManager.displayUsers();
                    break;
                case 2:
                    userManager.registerUser();
                    break;
                case 3:
                    userManager.editUser();
                    break;
                case 4:
                    userManager.deleteUser();
                    break;
                case 5:
                    return;
                default:
                    cout << "Invalid choice. Try again.\n";
                }
            }
            break;
        }
        case 4:
            return;
        default:
            cout << "Invalid choice. Try again.\n";
        }
    }
}

// Implementation of Renter::displayMenu
void Renter::displayMenu(RentalManager& rentalManager) {
    int choice;
    while (true) {
        cout << "\nRenter Menu:\n";
        cout << "1. Browse Items\n";
        cout << "2. Reserve Item\n";
        cout << "3. View Rental History\n";
        cout << "4. Exit to Main Menu\n";
        cout << "Choose an option: ";
        cin >> choice;

        switch (choice) {
        case 1:
            rentalManager.browseItems();
            break;
        case 2: {
            string itemName;
            cout << "Enter item name to reserve: ";
            cin >> itemName;
            rentalManager.reserveItem(email, itemName);
            break;
        }
        case 3:
            rentalManager.viewRentalHistory(email);
            break;
        case 4:
            return;
        default:
            cout << "Invalid choice. Try again.\n";
        }
    }
}

// Main Function
int main() {
    ensureFileExists("users.txt");
    ensureFileExists("items.txt");
    ensureFileExists("rental_history.txt");

    RentalManager rentalManager;
    rentalManager.loadItems();
    rentalManager.loadRentalHistory();

    UserManager userManager;
    userManager.loadUsers();

    int choice;
    while (true) {
        cout << "\nMain Menu:\n";
        cout << "1. Register\n";
        cout << "2. Login\n";
        cout << "3. Exit\n";
        cout << "Choose an option: ";
        cin >> choice;

        if (choice == 1) {
            userManager.registerUser();
        }
        else if (choice == 2) {
            string email, password;
            cout << "Enter Email: ";
            cin >> email;
            cout << "Enter Password: ";
            cin >> password;
            User* user = userManager.authenticate(email, password);
            if (user) {
                user->displayMenu(rentalManager);
            }
            else {
                cout << "Authentication failed. Please try again.\n";
            }
        }
        else if (choice == 3) {
            cout << "Exiting program.\n";
            break;
        }
        else {
            cout << "Invalid choice. Try again.\n";
        }
    }
    return 0;
}
