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
    string unit; // одиниця вимірювання (шт., кг, упаковка тощо)
};

// Вузол однозв'язного списку
// Реалізація структури даних у вигляді списку
struct Node {
    Product data;
    Node* next;
};

// ==============================================================================
// Допоміжні функції для контролю помилок користувача при введенні даних
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

// Функція, яка обчислює загальну вартість товару
double calculateTotalCost(const Product& p) {
    return p.unitPrice * p.quantity;
}

// ==============================================================================
// Основні функції (Логічно закінчені фрагменти оформлені у вигляді підпрограм)
// ==============================================================================

// добавлення елементу (коректування списку)
void addNode(Node*& head) {
    Product newProduct;
    newProduct.name = getValidString("Enter product name: ");
    newProduct.storeName = getValidString("Enter store name: ");
    newProduct.unitPrice = getValidDouble("Enter unit price (UAH): ");
    newProduct.quantity = getValidDouble("Enter quantity: ");
    newProduct.unit = getValidString("Enter unit of measurement (e.g., pcs, kg): ");

    Node* newNode = new Node{ newProduct, nullptr };

    if (!head) {
        head = newNode;
    }
    else {
        Node* current = head;
        while (current->next) {
            current = current->next;
        }
        current->next = newNode;
    }
    cout << "Product added successfully.\n";
}

// вилучення елементу (коректування списку)
void removeNode(Node*& head) {
    if (!head) {
        cout << "The list is empty.\n";
        return;
    }

    string targetName = getValidString("Enter the exact name of the product to remove: ");

    if (head->data.name == targetName) {
        Node* temp = head;
        head = head->next;
        delete temp;
        cout << "Product removed successfully.\n";
        return;
    }

    Node* current = head;
    while (current->next && current->next->data.name != targetName) {
        current = current->next;
    }

    if (current->next) {
        Node* temp = current->next;
        current->next = current->next->next;
        delete temp;
        cout << "Product removed successfully.\n";
    }
    else {
        cout << "Product not found.\n";
    }
}

// редагування інформації (коректування списку)
void editNode(Node* head) {
    if (!head) {
        cout << "The list is empty.\n";
        return;
    }

    string targetName = getValidString("Enter the exact name of the product to edit: ");
    Node* current = head;

    while (current) {
        if (current->data.name == targetName) {
            cout << "Editing product: " << targetName << "\n";
            current->data.name = getValidString("Enter new product name: ");
            current->data.storeName = getValidString("Enter new store name: ");
            current->data.unitPrice = getValidDouble("Enter new unit price (UAH): ");
            current->data.quantity = getValidDouble("Enter new quantity: ");
            current->data.unit = getValidString("Enter new unit of measurement: ");
            cout << "Product updated successfully.\n";
            return;
        }
        current = current->next;
    }
    cout << "Product not found.\n";
}

// перегляд
void printList(const Node* head) {
    if (!head) {
        cout << "The list is empty.\n";
        return;
    }

    cout << string(80, '-') << "\n";
    cout << left << setw(20) << "Product" << setw(20) << "Store"
        << setw(15) << "Price(UAH)" << setw(15) << "Quantity" << "Unit\n";
    cout << string(80, '-') << "\n";

    const Node* current = head;
    while (current) {
        cout << left << setw(20) << current->data.name
            << setw(20) << current->data.storeName
            << setw(15) << current->data.unitPrice
            << setw(15) << current->data.quantity
            << current->data.unit << "\n";
        current = current->next;
    }
    cout << string(80, '-') << "\n";
}

// сортування списку (сортування за назвою магазину або за загальною вартістю)
void sortList(Node*& head) {
    if (!head || !head->next) return;

    int choice;
    cout << "Sort by:\n1. Store Name\n2. Total Cost\nEnter choice: ";
    cin >> choice;

    bool swapped;
    Node* ptr1;
    Node* lptr = nullptr;

    do {
        swapped = false;
        ptr1 = head;

        while (ptr1->next != lptr) {
            bool needSwap = false;
            if (choice == 1) {
                if (ptr1->data.storeName > ptr1->next->data.storeName) needSwap = true;
            }
            else if (choice == 2) {
                if (calculateTotalCost(ptr1->data) > calculateTotalCost(ptr1->next->data)) needSwap = true;
            }

            if (needSwap) {
                swap(ptr1->data, ptr1->next->data);
                swapped = true;
            }
            ptr1 = ptr1->next;
        }
        lptr = ptr1;
    } while (swapped);

    cout << "List sorted successfully.\n";
}

// вивід на екран інформації про товари, що продаються в магазині, назва якого введена з клавіатури
void printByStore(const Node* head) {
    string targetStore = getValidString("Enter store name to search: ");
    bool found = false;

    const Node* current = head;
    while (current) {
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
    if (!found) cout << "No products found in this store.\n";
}

// вивід на екран інформації про товари із заданого з клавіатури діапазону вартості
void printByPriceRange(const Node* head) {
    double minPrice = getValidDouble("Enter minimum unit price: ");
    double maxPrice = getValidDouble("Enter maximum unit price: ");

    if (minPrice > maxPrice) {
        cout << "Invalid range.\n";
        return;
    }

    bool found = false;
    const Node* current = head;
    while (current) {
        if (current->data.unitPrice >= minPrice && current->data.unitPrice <= maxPrice) {
            if (!found) {
                cout << "Products in price range [" << minPrice << " - " << maxPrice << "]:\n";
                found = true;
            }
            cout << "- " << current->data.name << " at " << current->data.unitPrice << " UAH\n";
        }
        current = current->next;
    }
    if (!found) cout << "No products found in this price range.\n";
}

// збереження всієї структури даних у файл
void saveToFile(const Node* head) {
    string filename = getValidString("Enter filename to save (e.g., data.txt): ");
    ofstream outFile(filename);

    if (!outFile) {
        cout << "Error opening file for writing.\n";
        return;
    }

    const Node* current = head;
    while (current) {
        outFile << current->data.name << '\n'
            << current->data.storeName << '\n'
            << current->data.unitPrice << '\n'
            << current->data.quantity << '\n'
            << current->data.unit << '\n';
        current = current->next;
    }
    outFile.close();
    cout << "Data saved to " << filename << " successfully.\n";
}

// завантаження з файлу
void loadFromFile(Node*& head) {
    string filename = getValidString("Enter filename to load (e.g., data.txt): ");
    ifstream inFile(filename);

    if (!inFile) {
        cout << "Error opening file for reading.\n";
        return;
    }

    // Очищення поточного списку перед завантаженням
    while (head) {
        Node* temp = head;
        head = head->next;
        delete temp;
    }

    Product p;
    string line;
    Node* tail = nullptr;

    while (getline(inFile, p.name)) {
        getline(inFile, p.storeName);
        inFile >> p.unitPrice >> p.quantity;
        inFile >> ws; // clear whitespace/newline
        getline(inFile, p.unit);

        Node* newNode = new Node{ p, nullptr };
        if (!head) {
            head = newNode;
            tail = head;
        }
        else {
            tail->next = newNode;
            tail = newNode;
        }
    }
    inFile.close();
    cout << "Data loaded from " << filename << " successfully.\n";
}

// Очищення пам'яті перед виходом
void freeList(Node*& head) {
    while (head) {
        Node* temp = head;
        head = head->next;
        delete temp;
    }
}

// ==============================================================================
// Точка входу
// ==============================================================================

int main() {
    ofstream testFile("products.txt");
    if (testFile) {
        testFile << "Laptop\nTechStore\n25000.50\n10\npcs.\n";
        testFile << "Sugar\nATB\n32.40\n100\nkg\n";
        testFile << "Coffee\nSilpo\n150.00\n50\npack of 250g\n";
        testFile.close();
    } 
    Node* head = nullptr; // Всі необхідні дані передаються через список параметрів
    int choice = 0;

    // Програма має містити меню
    do {
        cout << "\n=== MAIN MENU ===\n";
        cout << "1. Add product\n";
        cout << "2. Remove product\n";
        cout << "3. Edit product\n";
        cout << "4. View all products\n";
        cout << "5. Sort list\n";
        cout << "6. View products by store\n";
        cout << "7. View products by price range\n";
        cout << "8. Save to file\n";
        cout << "9. Load from file\n";
        cout << "0. Exit\n";
        cout << "Enter your choice: ";

        if (!(cin >> choice)) {
            cout << "Invalid input. Please enter a number.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }

        switch (choice) {
        case 1: addNode(head); break;
        case 2: removeNode(head); break;
        case 3: editNode(head); break;
        case 4: printList(head); break;
        case 5: sortList(head); break;
        case 6: printByStore(head); break;
        case 7: printByPriceRange(head); break;
        case 8: saveToFile(head); break;
        case 9: loadFromFile(head); break;
        case 0: freeList(head); cout << "Exiting program...\n"; break;
        default: cout << "Unknown choice. Try again.\n";
        }
    } while (choice != 0);

    return 0;
}