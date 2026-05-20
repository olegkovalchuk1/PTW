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
// ЗАКРИТА ХЕШ-ТАБЛИЦЯ: Структури та Константи
// ==============================================================================

// ЗАКРИТА ХЕШ-ТАБЛИЦЯ: Стан комірки. Потрібен для того, щоб не ламати пошук при видаленні
enum SlotState { EMPTY, OCCUPIED, DELETED };

struct HashNode {
    Product data;
    SlotState state;

    HashNode() : state(EMPTY) {} // За замовчуванням всі комірки порожні
};

const int TABLE_SIZE = 101; // Розмір таблиці (бажано просте число)
HashNode hashTable[TABLE_SIZE]; // ЗАКРИТА ХЕШ-ТАБЛИЦЯ: Сама таблиця — це суцільний масив

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
// Основні функції закритої хеш-таблиці
// ==============================================================================

// Хеш-функція (генерує початковий індекс для рядка)
int hashFunction(const string& key) {
    long long hashVal = 0;
    for (char c : key) {
        hashVal = (hashVal * 31 + c) % TABLE_SIZE;
    }
    return hashVal;
}

// ЗАКРИТА ХЕШ-ТАБЛИЦЯ: Пошук індексу товару (з лінійним зондуванням)
int searchProductIndex(const string& name) {
    int index = hashFunction(name);
    int startIndex = index; // Щоб уникнути нескінченного циклу, якщо таблиця повна

    while (hashTable[index].state != EMPTY) {
        // Якщо комірка зайнята і імена збігаються - знайшли
        if (hashTable[index].state == OCCUPIED && hashTable[index].data.name == name) {
            return index;
        }
        // Якщо сталася колізія, або комірка має статус DELETED, йдемо далі
        index = (index + 1) % TABLE_SIZE;

        if (index == startIndex) break; // Обійшли всю таблицю
    }
    return -1; // Не знайдено
}

// ЗАКРИТА ХЕШ-ТАБЛИЦЯ: Вставка з лінійним вирішенням колізій
void insertProduct(const Product& p) {
    if (searchProductIndex(p.name) != -1) {
        cout << "Product with this name already exists!\n";
        return;
    }

    int index = hashFunction(p.name);
    int startIndex = index;

    // Шукаємо першу порожню АБО видалену комірку
    while (hashTable[index].state == OCCUPIED) {
        index = (index + 1) % TABLE_SIZE; // Крок на 1 вперед

        if (index == startIndex) {
            cout << "Hash table is full! Cannot add product.\n";
            return;
        }
    }

    // Записуємо дані у знайдений слот
    hashTable[index].data = p;
    hashTable[index].state = OCCUPIED;
    cout << "Product added successfully.\n";
}

void addProduct() {
    Product newProduct;
    newProduct.name = getValidString("Enter product name: ");

    if (searchProductIndex(newProduct.name) != -1) {
        cout << "Product already exists!\n";
        return;
    }

    newProduct.storeName = getValidString("Enter store name: ");
    newProduct.unitPrice = getValidDouble("Enter unit price (UAH): ");
    newProduct.quantity = getValidDouble("Enter quantity: ");
    newProduct.unit = getValidString("Enter unit of measurement: ");

    insertProduct(newProduct);
}

// ЗАКРИТА ХЕШ-ТАБЛИЦЯ: Видалення шляхом встановлення маркера DELETED
void removeProduct() {
    string targetName = getValidString("Enter the exact name of the product to remove: ");
    int index = searchProductIndex(targetName);

    if (index != -1) {
        hashTable[index].state = DELETED; // "Надгробок" зберігає ланцюг зондування
        cout << "Product removed successfully.\n";
    }
    else {
        cout << "Product not found.\n";
    }
}

void editProduct() {
    string targetName = getValidString("Enter the exact name of the product to edit: ");
    int index = searchProductIndex(targetName);

    if (index != -1) {
        cout << "Editing product: " << targetName << "\n";
        hashTable[index].data.storeName = getValidString("Enter new store name: ");
        hashTable[index].data.unitPrice = getValidDouble("Enter new unit price (UAH): ");
        hashTable[index].data.quantity = getValidDouble("Enter new quantity: ");
        hashTable[index].data.unit = getValidString("Enter new unit of measurement: ");
        cout << "Product updated successfully.\n";
    }
    else {
        cout << "Product not found.\n";
    }
}

// ==============================================================================
// Функції виводу (сканування всього масиву)
// ==============================================================================

void viewAllProducts() {
    cout << string(80, '-') << "\n";
    cout << left << setw(20) << "Product" << setw(20) << "Store"
        << setw(15) << "Price(UAH)" << setw(15) << "Quantity" << "Unit\n";
    cout << string(80, '-') << "\n";

    bool isEmpty = true;
    // ЗАКРИТА ХЕШ-ТАБЛИЦЯ: Для виводу ми просто лінійно скануємо весь масив
    for (int i = 0; i < TABLE_SIZE; i++) {
        if (hashTable[i].state == OCCUPIED) {
            isEmpty = false;
            cout << left << setw(20) << hashTable[i].data.name
                << setw(20) << hashTable[i].data.storeName
                << setw(15) << hashTable[i].data.unitPrice
                << setw(15) << hashTable[i].data.quantity
                << hashTable[i].data.unit << "\n";
        }
    }

    if (isEmpty) cout << "The Hash Table is empty.\n";
    cout << string(80, '-') << "\n";
}

void viewProductsByStore() {
    string targetStore = getValidString("Enter store name to search: ");
    bool found = false;

    for (int i = 0; i < TABLE_SIZE; i++) {
        if (hashTable[i].state == OCCUPIED && hashTable[i].data.storeName == targetStore) {
            if (!found) {
                cout << "Products in " << targetStore << ":\n";
                found = true;
            }
            cout << "- " << hashTable[i].data.name << " (" << hashTable[i].data.quantity
                << " " << hashTable[i].data.unit << ") at " << hashTable[i].data.unitPrice << " UAH\n";
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
        if (hashTable[i].state == OCCUPIED &&
            hashTable[i].data.unitPrice >= minPrice &&
            hashTable[i].data.unitPrice <= maxPrice) {
            if (!found) {
                cout << "Products in price range [" << minPrice << " - " << maxPrice << "]:\n";
                found = true;
            }
            cout << "- " << hashTable[i].data.name << " at " << hashTable[i].data.unitPrice << " UAH\n";
        }
    }
    if (!found) cout << "No products found in this price range.\n";
}

// ==============================================================================
// Робота з файлами
// ==============================================================================

void saveToFile() {
    string filename = getValidString("Enter filename to save (e.g., hash.txt): ");
    ofstream outFile(filename);
    if (!outFile) {
        cout << "Error opening file.\n";
        return;
    }

    for (int i = 0; i < TABLE_SIZE; i++) {
        if (hashTable[i].state == OCCUPIED) {
            outFile << hashTable[i].data.name << '\n'
                << hashTable[i].data.storeName << '\n'
                << hashTable[i].data.unitPrice << '\n'
                << hashTable[i].data.quantity << '\n'
                << hashTable[i].data.unit << '\n';
        }
    }
    outFile.close();
    cout << "Data saved successfully.\n";
}

void loadFromFile() {
    string filename = getValidString("Enter filename to load (e.g., hash.txt): ");
    ifstream inFile(filename);
    if (!inFile) {
        cout << "Error opening file.\n";
        return;
    }

    // Очищення таблиці перед завантаженням
    for (int i = 0; i < TABLE_SIZE; i++) {
        hashTable[i].state = EMPTY;
    }

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
        cout << "\n=== CLOSED HASH TABLE MENU ===\n";
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
        case 0: cout << "Exiting...\n"; break;
        default: cout << "Unknown choice. Try again.\n";
        }
    } while (choice != 0);

    return 0;
}