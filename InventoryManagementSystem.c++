
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <iomanip>
#include <limits>
#include <algorithm>

using namespace std;

const string DATA_FILE = "inventory.dat";

// ============================================================
// Product Structure
// ============================================================
struct Product {
    int    id;
    string name;
    int    quantity;
    float  price;
};

// ============================================================
//  File : Save & Load
// ============================================================

void saveToFile(const vector<Product>& inventory) {
    ofstream file(DATA_FILE);
    if (!file) {
        cout << "Warning: Could not save to file.\n";
        return;
    }
    for (const auto& p : inventory) {
        file << p.id << "|" << p.name << "|" << p.quantity << "|" << p.price << "\n";
    }
    file.close();
}

void loadFromFile(vector<Product>& inventory) {
    ifstream file(DATA_FILE);
    if (!file) return; 

    string line;

    while (getline(file, line)) {
        if (line.empty()) continue;

        Product p;

        try {
            size_t pos;

            pos = line.find('|');
            p.id = stoi(line.substr(0, pos));
            line.erase(0, pos + 1);

            pos = line.find('|');
            p.name = line.substr(0, pos);
            line.erase(0, pos + 1);

            pos = line.find('|');
            p.quantity = stoi(line.substr(0, pos));
            line.erase(0, pos + 1);

            p.price = stof(line);

            inventory.push_back(p);
        }
        catch (const exception& e) {
            continue;
        }
    }

    file.close();
}
// ============================================================
// INPUT VALIDATION HELPERS
// ============================================================

int getValidInt(const string& prompt, int minVal = 0) {
    int value;
    while (true) {
        cout << prompt;
        cin >> value;
        if (cin.fail() || value < minVal) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid input! Please enter a number";
            if (minVal > 0) cout << " >= " << minVal;
            cout << ".\n";
        } else {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return value;
        }
    }
}

// Safe float input — keeps asking until valid
float getValidFloat(const string& prompt, float minVal = 0.0f) {
    float value;
    while (true) {
        cout << prompt;
        cin >> value;
        if (cin.fail() || value < minVal) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid price! Enter a positive number.\n";
        } else {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return value;
        }
    }
}

// Safe non-empty string input
string getValidString(const string& prompt) {
    string value;
    while (true) {
        cout << prompt;
        getline(cin, value);
        // Trim leading/trailing spaces
        size_t start = value.find_first_not_of(" \t");
        size_t end   = value.find_last_not_of(" \t");
        if (start == string::npos) {
            cout << "Name cannot be empty!\n";
        } else {
            return value.substr(start, end - start + 1);
        }
    }
}

// ============================================================
// DISPLAY HELPERS
// ============================================================

void printDivider(char c = '-', int width = 55) {
    cout << string(width, c) << endl;
}

void printHeader() {
    printDivider('=');
    cout << left
         << setw(6)  << "ID"
         << setw(22) << "Name"
         << setw(10) << "Qty"
         << "Price"
         << endl;
    printDivider('=');
}

void printProduct(const Product& p) {
    cout << left
         << setw(6)  << p.id
         << setw(22) << p.name
         << setw(10) << p.quantity
         << "$" << fixed << setprecision(2) << p.price;

    if (p.quantity < 5) { //low stock warning!!
        cout << "Low stock!";
    }
    cout << endl;
}

// ============================================================
// 1. ADD PRODUCT
// ============================================================
//Because the container order is not guaranteed after 
// operations like sorting or deletion->compute the maximum ID dynamically to ensure uniqueness.
int getNextId(const vector<Product>& inventory) {
    int maxId = 0;
    for (const auto& p : inventory) {
        if (p.id > maxId) {
            maxId = p.id;
        }
    }
    return maxId + 1;
}
void addProduct(vector<Product>& inventory) {
    cout << "\n📦 --- Add New Product ---\n";

    Product p;

    // ID generation
    p.id = getNextId(inventory);
    cout << "Generated Product ID: " << p.id << endl;

    // Input
    p.name     = getValidString("Enter Product Name : ");
    p.quantity = getValidInt("Enter Quantity     : ", 0);
    p.price    = getValidFloat("Enter Price ($)    : ", 0.01f);

    // Add to inventory
    inventory.push_back(p);

    //Save automatically
    saveToFile(inventory);

    cout << "Product \"" << p.name << "\" added successfully!\n";
}

// ============================================================
// 2. DISPLAY ALL PRODUCTS
// ============================================================
void displayProducts(const vector<Product>& inventory) {
    cout << "\n📋 --- All Products ---\n";

    if (inventory.empty()) {
        cout << "⚠️  No products in inventory. Add some first!\n";
        return;
    }

    printHeader();
    for (const auto& p : inventory) {
        printProduct(p);
    }
    printDivider();

    // Summary stats
    float total = 0;
    int   totalQty = 0;
    for (const auto& p : inventory) {
        total    += p.price * p.quantity;
        totalQty += p.quantity;
    }
    cout << "Total products : " << inventory.size() << endl;
    cout << "Total items    : " << totalQty << endl;
    cout << "Inventory value: $" << fixed << setprecision(2) << total << endl;
    printDivider();
}

// ============================================================
// 3. SEARCH PRODUCT BY ID OR NAME
// ============================================================
void searchProduct(const vector<Product>& inventory) {
    cout << "\n🔍 --- Search Product ---\n";

    if (inventory.empty()) {
        cout << "⚠️ No products available right now.\n";
        return;
    }

    cout << "Search by:\n";
    cout << "  1. Product ID\n";
    cout << "  2. Product Name\n";

    int choice = getValidInt("Your choice: ", 1);
    bool found = false;

    if (choice == 1) {
        int id = getValidInt("Enter Product ID: ", 1);
        for (const auto& p : inventory) {
            if (p.id == id) {
                printHeader();
                printProduct(p);
                printDivider();
                found = true;
                break;
            }
        }
    } 
    else if (choice == 2) {
        string keyword = getValidString("Enter product name (or part of it): ");

        string kwLower = keyword;
        transform(kwLower.begin(), kwLower.end(), kwLower.begin(), ::tolower);

        bool headerPrinted = false;

        for (const auto& p : inventory) {
            string nameLower = p.name;
            transform(nameLower.begin(), nameLower.end(), nameLower.begin(), ::tolower);

            if (nameLower.find(kwLower) != string::npos) {

                if (!headerPrinted) {
                    printHeader();
                    headerPrinted = true;
                }

                printProduct(p);
                found = true;
            }
        }

        if (found) printDivider(); 
    } 
    else {
        cout << "Invalid choice.\n";
        return;
    }

    if (!found) cout << "No product found.\n";
}

// ============================================================
// 4. UPDATE QUANTITY
// ============================================================
void updateProduct(vector<Product>& inventory) {
    cout << "\n✏️  --- Update Product ---\n";

    if (inventory.empty()) {
        cout << "No products to update.\n";
        return;
    }

    int id = getValidInt("Enter Product ID to update: ", 1);

    for (auto& p : inventory) {
        if (p.id == id) {
            cout << "Product found: \"" << p.name << "\"\n";
            cout << "  Current Quantity : " << p.quantity << "\n";
            cout << "  Current Price    : $" << fixed << setprecision(2) << p.price << "\n";
            cout << "\nWhat would you like to update?\n";
            cout << "  1. Quantity\n";
            cout << "  2. Price\n";
            cout << "  3. Both\n";
            int choice = getValidInt("Your choice: ", 1);

            if (choice == 1 || choice == 3) {
                p.quantity = getValidInt("Enter new quantity: ", 0);
            }
            if (choice == 2 || choice == 3) {
                p.price = getValidFloat("Enter new price ($): ", 0.01f);
            }

            saveToFile(inventory); // 💾 Auto-save
            cout << "Product updated successfully!\n";
            return;
        }
    }
    cout << "Product with ID " << id << " not found.\n";
}

// ============================================================
// 5. DELETE PRODUCT
// ============================================================
void deleteProduct(vector<Product>& inventory) {
    cout << "\n🗑️  --- Delete Product ---\n";

    if (inventory.empty()) {
        cout << "No products to delete.\n";
        return;
    }

    int id = getValidInt("Enter Product ID to delete: ", 1);

    for (auto it = inventory.begin(); it != inventory.end(); ++it) {
        if (it->id == id) {
            cout << "Are you sure you want to delete \"" << it->name << "\"? (y/n): ";
            char confirm;

            cin >> confirm;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');

            if (confirm == 'y' || confirm == 'Y') {
                inventory.erase(it);
                saveToFile(inventory); // 💾 Auto-save
                cout << "Product deleted successfully!\n";
            } else {
                cout << "Deletion cancelled.\n";
            }
            return;
        }
    }
    cout << "Product with ID " << id << " not found.\n";
}

// ============================================================
// 6. SORT PRODUCTS
// ============================================================
void sortProducts(vector<Product>& inventory) {
    cout << "\n📊 --- Sort Products ---\n";
    cout << "1. Price (Low → High)\n";
    cout << "2. Quantity (Low → High)\n";

    int choice = getValidInt("Your choice: ", 1);

    if (choice == 1) {
        sort(inventory.begin(), inventory.end(),
             [](const Product& a, const Product& b) {
                 return a.price < b.price;
             });

        cout << "Sorted by price.\n";
        saveToFile(inventory);
    }
    else if (choice == 2) {
        sort(inventory.begin(), inventory.end(),
             [](const Product& a, const Product& b) {
                 return a.quantity < b.quantity;
             });

        cout << "Sorted by quantity.\n";
        saveToFile(inventory); // 🔥 ADD THIS
    }
    else {
        cout << "Invalid choice.\n";
    }
}

// ============================================================
// MENU
// ============================================================
void showMenu() {
    cout << "\n";
    printDivider('=');
    cout << "     🏪  INVENTORY MANAGEMENT SYSTEM PRO\n";
    printDivider('=');
    cout << "  1. 📦  Add Product\n";
    cout << "  2. 📋  View All Products\n";
    cout << "  3. 🔍  Search Product\n";
    cout << "  4. ✏️   Update Product\n";
    cout << "  5. 🗑️   Delete Product\n";
    cout << "  6. 📊  Sort Products\n";
    cout << "  7. 🚪  Exit\n";
    printDivider('=');
    cout << "  Your choice: ";
}

// ============================================================
// MAIN
// ============================================================
int main() {
    vector<Product> inventory;

    // 💾 Load saved data on startup
    loadFromFile(inventory);

    if (inventory.empty()) {
        cout << "No saved data found. Starting fresh!\n";
    } else {
        cout << "💾 Loaded " << inventory.size() << " product(s) from file.\n";
    }

    int choice;
    while (true) {
        showMenu();
        cin >> choice;

        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid input! Please enter 1–7.\n";
            continue;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        switch (choice) {
            case 1: addProduct(inventory);    break;
            case 2: displayProducts(inventory); break;
            case 3: searchProduct(inventory); break;
            case 4: updateProduct(inventory); break;
            case 5: deleteProduct(inventory); break;
            case 6: sortProducts(inventory) ;break;
            case 7:
                cout << "\n👋 Goodbye! Your inventory has been saved.\n\n";
                return 0;
            default:
                cout << "Invalid input! Please enter 1–7.\n";
        }
    }
}