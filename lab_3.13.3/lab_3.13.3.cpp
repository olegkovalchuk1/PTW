#include <iostream>
#include <string>
#include <fstream>
#include <limits>
#include <iomanip>

using namespace std;

// Структура, що описує товар
struct Product {
    string name;
    string storeName;
    double unitPrice;
    double quantity;
    string unit;
};

// ==============================================================================
// ВІДКРИТА ХЕШ-ТАБЛИЦЯ: Структури та Константи
// ==============================================================================

struct HashNode {
    Product data;
    // ВІДКРИТА ХЕШ-ТАБЛИЦЯ: Вказівник на наступний елемент для побудови ланцюжка (списку)
    HashNode* next;

    HashNode(Product p) : data(p), next(nullptr) {}
};

const int TABLE_SIZE = 101;
// ВІДКРИТА ХЕШ-ТАБЛИЦЯ: Масив вказівників. Кожна комірка — це голова окремого зв'язного списку
HashNode* hashTable[TABLE_SIZE] = { nullptr };

// ==============================================================================
// Допоміжні функції контролю вводу
// ==============================================================================

string getValidString(const string& prompt) {
    string input;
    cout << prompt;
    getline(cin >> ws, input);
    return input;
}

double getValidDouble(const string& prompt) {
    double value;
    while (true) {
        cout << prompt;
        if (cin >> value && value >= 0) {
            return value;
        }
        cout << "Invalid input. Please enter a positive number.\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}

// ==============================================================================
// Основні функції відкритої хеш-таблиці
// ==============================================================================

// Хеш-функція (генерує індекс масиву для рядка)
int hashFunction(const string& key) {
    long long hashVal = 0;
    for (char c : key) {
        hashVal = (hashVal * 31 + c) % TABLE_SIZE;
    }
    return hashVal;
}

// ВІДКРИТА ХЕШ-ТАБЛИЦЯ: Пошук товару. Обчислюємо індекс і йдемо по ланцюжку (списку)
HashNode* searchProductNode(const string& name) {
    int index = hashFunction(name);
    HashNode* current = hashTable[index];

    while (current != nullptr) {
        if (current->data.name == name) {
            return current;
        }
        current = current->next; // Рухаємось по ланцюжку колізій
    }
    return nullptr;
}

// ВІДКРИТА ХЕШ-ТАБЛИЦЯ: Вставка. Додаємо новий вузол на початок списку у відповідній комірці
void insertProduct(const Product& p) {
    if (searchProductNode(p.name) != nullptr) {
        cout << "Product with this name already exists!\n";
        return;
    }

    int index = hashFunction(p.name);
    HashNode* newNode = new HashNode(p);

    // Вставляємо в голову списку (швидка вставка O(1) для даного індексу)
    newNode->next = hashTable[index];
    hashTable[index] = newNode;

    cout << "Product added successfully.\n";
}

void addProduct() {
    Product newProduct;
    newProduct.name = getValidString("Enter product name: ");

    if (searchProductNode(newProduct.name) != nullptr) {
        cout << "Product already exists!\n";
        return;
    }

    newProduct.storeName = getValidString("Enter store name: ");
    newProduct.unitPrice = getValidDouble("Enter unit price (UAH): ");
    newProduct.quantity = getValidDouble("Enter quantity: ");
    newProduct.unit = getValidString("Enter unit of measurement: ");

    insertProduct(newProduct);
}

// ВІДКРИТА ХЕШ-ТАБЛИЦЯ: Видалення. Класичне видалення вузла з однозв'язного списку
void removeProduct() {
    string targetName = getValidString("Enter the exact name of the product to remove: ");
    int index = hashFunction(targetName);

    HashNode* current = hashTable[index];
    HashNode* prev = nullptr;

    while (current != nullptr && current->data.name != targetName) {
        prev = current;
        current = current->next;
    }

    if (current == nullptr) {
        cout << "Product not found.\n";
        return;
    }

    // Відв'язуємо знайдений вузол від ланцюжка
    if (prev == nullptr) {
        hashTable[index] = current->next; // Видаляємо голову списку
    }
    else {
        prev->next = current->next;       // Видаляємо з середини або кінця
    }

    delete current; // Звільняємо пам'ять
    cout << "Product removed successfully.\n";
}

void editProduct() {
    string targetName = getValidString("Enter the exact name of the product to edit: ");
    HashNode* targetNode = searchProductNode(targetName);

    if (targetNode != nullptr) {
        cout << "Editing product: " << targetName << "\n";
        targetNode->data.storeName = getValidString("Enter new store name: ");
        targetNode->data.unitPrice = getValidDouble("Enter new unit price (UAH): ");
        targetNode->data.quantity = getValidDouble("Enter new quantity: ");
        targetNode->data.unit = getValidString("Enter new unit of measurement: ");
        cout << "Product updated successfully.\n";
    }
    else {
        cout << "Product not found.\n";
    }
}

// ==============================================================================
// Функції виводу (сканування масиву та всіх його ланцюжків)
// ==============================================================================

void viewAllProducts() {
    cout << string(80, '-') << "\n";
    cout << left << setw(20) << "Product" << setw(20) << "Store"
        << setw(15) << "Price(UAH)" << setw(15) << "Quantity" << "Unit\n";
    cout << string(80, '-') << "\n";

    bool isEmpty = true;

    // ВІДКРИТА ХЕШ-ТАБЛИЦЯ: Проходимо по кожному індексу масиву...
    for (int i = 0; i < TABLE_SIZE; i++) {
        HashNode* current = hashTable[i];
        // ...і по всьому ланцюжку (списку) в кожному індексі
        while (current != nullptr) {
            isEmpty = false;
            cout << left << setw(20) << current->data.name
                << setw(20) << current->data.storeName
                << setw(15) << current->data.unitPrice
                << setw(15) << current->data.quantity
                << current->data.unit << "\n";
            current = current->next;
        }
    }

    if (isEmpty) cout << "The Hash Table is empty.\n";
    cout << string(80, '-') << "\n";
}

void viewProductsByStore() {
    string targetStore = getValidString("Enter store name to search: ");
    bool found = false;

    for (int i = 0; i < TABLE_SIZE; i++) {
        HashNode* current = hashTable[i];
        while (current != nullptr) {
            if (current->data.storeName == targetStore) {
                if (!found) {
                    cout << "Products in " << targetStore << ":\n";
                    found = true;
                }
                cout << "- " << current->data.name << " (" << current->data.quantity
                    << " " << current->data.unit << ") at " << current->data.unitPrice << " UAH\n";
            }
            current = current->next;
        }
    }
    if (!found) cout << "No products found in this store.\n";
}

void viewProductsByPriceRange() {
    double minPrice = getValidDouble("Enter minimum unit price: ");
    double maxPrice = getValidDouble("Enter maximum unit price: ");
    if (minPrice > maxPrice) {
        cout << "Invalid range.\n";
        return;
    }
    bool found = false;

    for (int i = 0; i < TABLE_SIZE; i++) {
        HashNode* current = hashTable[i];
        while (current != nullptr) {
            if (current->data.unitPrice >= minPrice && current->data.unitPrice <= maxPrice) {
                if (!found) {
                    cout << "Products in price range [" << minPrice << " - " << maxPrice << "]:\n";
                    found = true;
                }
                cout << "- " << current->data.name << " at " << current->data.unitPrice << " UAH\n";
            }
            current = current->next;
        }
    }
    if (!found) cout << "No products found in this price range.\n";
}

// ==============================================================================
// Робота з файлами та очищення пам'яті
// ==============================================================================

void saveToFile() {
    string filename = getValidString("Enter filename to save (e.g., open_hash.txt): ");
    ofstream outFile(filename);
    if (!outFile) {
        cout << "Error opening file.\n";
        return;
    }

    for (int i = 0; i < TABLE_SIZE; i++) {
        HashNode* current = hashTable[i];
        while (current != nullptr) {
            outFile << current->data.name << '\n'
                << current->data.storeName << '\n'
                << current->data.unitPrice << '\n'
                << current->data.quantity << '\n'
                << current->data.unit << '\n';
            current = current->next;
        }
    }
    outFile.close();
    cout << "Data saved successfully.\n";
}

// ВІДКРИТА ХЕШ-ТАБЛИЦЯ: Функція для повного очищення всіх динамічних списків
void freeTable() {
    for (int i = 0; i < TABLE_SIZE; i++) {
        HashNode* current = hashTable[i];
        while (current != nullptr) {
            HashNode* toDelete = current;
            current = current->next;
            delete toDelete;
        }
        hashTable[i] = nullptr;
    }
}

void loadFromFile() {
    string filename = getValidString("Enter filename to load (e.g., open_hash.txt): ");
    ifstream inFile(filename);
    if (!inFile) {
        cout << "Error opening file.\n";
        return;
    }

    freeTable(); // Очищуємо старі списки перед завантаженням

    Product p;
    while (getline(inFile, p.name)) {
        getline(inFile, p.storeName);
        inFile >> p.unitPrice >> p.quantity;
        inFile >> ws;
        getline(inFile, p.unit);

        insertProduct(p);
    }
    inFile.close();
    cout << "Data loaded successfully.\n";
}

// ==============================================================================
// Точка входу
// ==============================================================================

int main() {
    ofstream testFile("tree_data.txt");
    if (testFile) {
        testFile << "Laptop\nTechStore\n25000\n10\npcs\n"
            << "Apple\nSilpo\n35\n100\nkg\n"
            << "Sugar\nATB\n32.40\n50\nkg\n";
        testFile.close();
    }
    int choice = 0;

    do {
        cout << "\n=== OPEN HASH TABLE MENU ===\n";
        cout << "1. Add product\n";
        cout << "2. Remove product\n";
        cout << "3. Edit product\n";
        cout << "4. View all products\n";
        cout << "5. View products by store\n";
        cout << "6. View products by price range\n";
        cout << "7. Save to file\n";
        cout << "8. Load from file\n";
        cout << "0. Exit\n";
        cout << "Enter your choice: ";

        if (!(cin >> choice)) {
            cout << "Invalid input. Please enter a number.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }

        switch (choice) {
        case 1: addProduct(); break;
        case 2: removeProduct(); break;
        case 3: editProduct(); break;
        case 4: viewAllProducts(); break;
        case 5: viewProductsByStore(); break;
        case 6: viewProductsByPriceRange(); break;
        case 7: saveToFile(); break;
        case 8: loadFromFile(); break;
        case 0: freeTable(); cout << "Exiting...\n"; break; // Не забуваємо звільнити пам'ять!
        default: cout << "Unknown choice. Try again.\n";
        }
    } while (choice != 0);

    return 0;
}