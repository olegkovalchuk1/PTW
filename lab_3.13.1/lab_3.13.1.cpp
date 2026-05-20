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
// Префіксне дерево (Trie): структура вузла
// ==============================================================================
const int ALPHABET_SIZE = 256; // Розмір алфавіту (всі ASCII символи)

struct TrieNode {
    // ПРЕФІКСНЕ ДЕРЕВО: масив вказівників на наступні вузли-символи
    TrieNode* children[ALPHABET_SIZE];

    // ПРЕФІКСНЕ ДЕРЕВО: прапорець, що вказує, чи закінчується тут ключ (назва товару)
    bool isEndOfWord;

    // ПРЕФІКСНЕ ДЕРЕВО: дані зберігаються лише у вузлах, де isEndOfWord == true
    Product data;

    TrieNode() {
        isEndOfWord = false;
        for (int i = 0; i < ALPHABET_SIZE; i++) {
            children[i] = nullptr;
        }
    }
};

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
// Основні функції префіксного дерева
// ==============================================================================

// ПРЕФІКСНЕ ДЕРЕВО: Пошук посимвольно (без порівняння цілих рядків)
TrieNode* searchNodeTrie(TrieNode* root, const string& key) {
    if (!root) return nullptr;

    TrieNode* current = root;
    for (char c : key) {
        int index = (unsigned char)c;
        if (!current->children[index]) {
            return nullptr; // Якщо гілки для символу немає — товар не знайдено
        }
        current = current->children[index];
    }

    // Повертаємо вузол лише якщо це дійсно кінець слова, а не проміжний префікс
    if (current && current->isEndOfWord) {
        return current;
    }
    return nullptr;
}

// ПРЕФІКСНЕ ДЕРЕВО: Додавання товару, прокладаючи шлях для кожного символу
void insertNodeTrie(TrieNode*& root, const Product& p) {
    if (!root) root = new TrieNode();

    TrieNode* current = root;
    for (char c : p.name) {
        int index = (unsigned char)c;
        if (!current->children[index]) {
            current->children[index] = new TrieNode(); // Створюємо новий вузол для символу
        }
        current = current->children[index];
    }

    if (current->isEndOfWord) {
        cout << "Product with this name already exists!\n";
    }
    else {
        current->isEndOfWord = true; // Маркуємо кінець ключа
        current->data = p;           // Записуємо дані у фінальний вузол
        cout << "Product added successfully.\n";
    }
}

void addProduct(TrieNode*& root) {
    Product newProduct;
    newProduct.name = getValidString("Enter product name: ");

    if (searchNodeTrie(root, newProduct.name)) {
        cout << "Product already exists!\n";
        return;
    }

    newProduct.storeName = getValidString("Enter store name: ");
    newProduct.unitPrice = getValidDouble("Enter unit price (UAH): ");
    newProduct.quantity = getValidDouble("Enter quantity: ");
    newProduct.unit = getValidString("Enter unit of measurement: ");

    insertNodeTrie(root, newProduct);
}

// Допоміжна функція перевірки, чи вузол має хоча б одного нащадка
bool hasChildren(TrieNode* node) {
    for (int i = 0; i < ALPHABET_SIZE; i++) {
        if (node->children[i]) return true;
    }
    return false;
}

// ПРЕФІКСНЕ ДЕРЕВО: Рекурсивне вилучення із зачисткою порожніх вузлів знизу вгору
TrieNode* removeNodeTrie(TrieNode* root, const string& key, int depth, bool& removed) {
    if (!root) return nullptr;

    // Якщо дійшли до кінця слова
    if (depth == key.length()) {
        if (root->isEndOfWord) {
            root->isEndOfWord = false; // Знімаємо маркер
            removed = true;
        }
        // Якщо у вузла немає продовжень (інших слів), видаляємо його
        if (!hasChildren(root)) {
            delete root;
            root = nullptr;
        }
        return root;
    }

    int index = (unsigned char)key[depth];
    root->children[index] = removeNodeTrie(root->children[index], key, depth + 1, removed);

    // Якщо після видалення гілки поточний вузол став порожнім і він не є кінцем іншого слова
    if (!hasChildren(root) && !root->isEndOfWord) {
        delete root;
        root = nullptr;
    }
    return root;
}

void removeProduct(TrieNode*& root) {
    if (!root) {
        cout << "The Trie is empty.\n";
        return;
    }
    string targetName = getValidString("Enter the exact name of the product to remove: ");
    bool removed = false;
    root = removeNodeTrie(root, targetName, 0, removed);

    if (removed) cout << "Product removed successfully.\n";
    else cout << "Product not found.\n";
}

void editProduct(TrieNode* root) {
    string targetName = getValidString("Enter the exact name of the product to edit: ");
    TrieNode* targetNode = searchNodeTrie(root, targetName);

    if (targetNode) {
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
// Функції виводу (обхід префіксного дерева)
// ==============================================================================

// ПРЕФІКСНЕ ДЕРЕВО: Лексикографічний обхід (автоматично сортує за алфавітом)
void printTrieTraversal(TrieNode* root) {
    if (!root) return;

    if (root->isEndOfWord) {
        cout << left << setw(20) << root->data.name
            << setw(20) << root->data.storeName
            << setw(15) << root->data.unitPrice
            << setw(15) << root->data.quantity
            << root->data.unit << "\n";
    }

    for (int i = 0; i < ALPHABET_SIZE; i++) {
        if (root->children[i]) {
            printTrieTraversal(root->children[i]);
        }
    }
}

void viewAllProducts(TrieNode* root) {
    if (!root) {
        cout << "The Trie is empty.\n";
        return;
    }
    cout << string(80, '-') << "\n";
    cout << left << setw(20) << "Product" << setw(20) << "Store"
        << setw(15) << "Price(UAH)" << setw(15) << "Quantity" << "Unit\n";
    cout << string(80, '-') << "\n";
    printTrieTraversal(root);
    cout << string(80, '-') << "\n";
}

void printByStoreRecursive(TrieNode* root, const string& storeName, bool& found) {
    if (!root) return;

    if (root->isEndOfWord && root->data.storeName == storeName) {
        if (!found) {
            cout << "Products in " << storeName << ":\n";
            found = true;
        }
        cout << "- " << root->data.name << " (" << root->data.quantity
            << " " << root->data.unit << ") at " << root->data.unitPrice << " UAH\n";
    }

    for (int i = 0; i < ALPHABET_SIZE; i++) {
        if (root->children[i]) printByStoreRecursive(root->children[i], storeName, found);
    }
}

void viewProductsByStore(TrieNode* root) {
    string targetStore = getValidString("Enter store name to search: ");
    bool found = false;
    printByStoreRecursive(root, targetStore, found);
    if (!found) cout << "No products found in this store.\n";
}

void printByPriceRangeRecursive(TrieNode* root, double minPrice, double maxPrice, bool& found) {
    if (!root) return;

    if (root->isEndOfWord && root->data.unitPrice >= minPrice && root->data.unitPrice <= maxPrice) {
        if (!found) {
            cout << "Products in price range [" << minPrice << " - " << maxPrice << "]:\n";
            found = true;
        }
        cout << "- " << root->data.name << " at " << root->data.unitPrice << " UAH\n";
    }

    for (int i = 0; i < ALPHABET_SIZE; i++) {
        if (root->children[i]) printByPriceRangeRecursive(root->children[i], minPrice, maxPrice, found);
    }
}

void viewProductsByPriceRange(TrieNode* root) {
    double minPrice = getValidDouble("Enter minimum unit price: ");
    double maxPrice = getValidDouble("Enter maximum unit price: ");
    if (minPrice > maxPrice) {
        cout << "Invalid range.\n";
        return;
    }
    bool found = false;
    printByPriceRangeRecursive(root, minPrice, maxPrice, found);
    if (!found) cout << "No products found in this price range.\n";
}

// ==============================================================================
// Робота з файлами та очищення
// ==============================================================================

void savePreOrderTrie(TrieNode* root, ofstream& outFile) {
    if (!root) return;

    if (root->isEndOfWord) {
        outFile << root->data.name << '\n'
            << root->data.storeName << '\n'
            << root->data.unitPrice << '\n'
            << root->data.quantity << '\n'
            << root->data.unit << '\n';
    }

    for (int i = 0; i < ALPHABET_SIZE; i++) {
        if (root->children[i]) {
            savePreOrderTrie(root->children[i], outFile);
        }
    }
}

void saveToFile(TrieNode* root) {
    string filename = getValidString("Enter filename to save (e.g., trie.txt): ");
    ofstream outFile(filename);
    if (!outFile) {
        cout << "Error opening file.\n";
        return;
    }
    savePreOrderTrie(root, outFile);
    outFile.close();
    cout << "Trie saved successfully.\n";
}

// ПРЕФІКСНЕ ДЕРЕВО: Очищення пам'яті (повне проходження по всіх масивах-гілках)
void freeTrie(TrieNode*& root) {
    if (!root) return;
    for (int i = 0; i < ALPHABET_SIZE; i++) {
        if (root->children[i]) {
            freeTrie(root->children[i]);
        }
    }
    delete root;
    root = nullptr;
}

void loadFromFile(TrieNode*& root) {
    string filename = getValidString("Enter filename to load (e.g., trie.txt): ");
    ifstream inFile(filename);
    if (!inFile) {
        cout << "Error opening file.\n";
        return;
    }

    freeTrie(root);

    Product p;
    while (getline(inFile, p.name)) {
        getline(inFile, p.storeName);
        inFile >> p.unitPrice >> p.quantity;
        inFile >> ws;
        getline(inFile, p.unit);

        insertNodeTrie(root, p);
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

    TrieNode* root = nullptr;
    int choice = 0;

    do {
        cout << "\n=== PREFIX TREE (TRIE) MENU ===\n";
        cout << "1. Add product\n";
        cout << "2. Remove product\n";
        cout << "3. Edit product\n";
        cout << "4. View all products (Lexicographical)\n";
        cout << "5. Balance Tree (Not needed for Tries)\n"; // Балансування для Trie позбавлене сенсу
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
        case 1: addProduct(root); break;
        case 2: removeProduct(root); break;
        case 3: editProduct(root); break;
        case 4: viewAllProducts(root); break;
        case 5: cout << "Notice: Prefix Trees (Tries) are inherently structured by characters and do not require balancing.\n"; break;
        case 6: viewProductsByStore(root); break;
        case 7: viewProductsByPriceRange(root); break;
        case 8: saveToFile(root); break;
        case 9: loadFromFile(root); break;
        case 0: freeTrie(root); cout << "Exiting...\n"; break;
        default: cout << "Unknown choice. Try again.\n";
        }
    } while (choice != 0);

    return 0;
}