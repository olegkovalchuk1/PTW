#include <iostream>
#include <string>
#include <fstream>
#include <limits>
#include <iomanip>
#include <cmath>

using namespace std;

// --- ÑÒĞÓÊÒÓĞÈ ÄÀÍÈÕ ÒÀ ÒÈÏÈ ---

struct Product {
    string name;
    string storeName;
    double unitPrice;
    int quantity;
    string unit;

    // Ôóíêö³ÿ îá÷èñëåííÿ âàğòîñò³ (íå âèâîäèòü, à ïîâåğòàº)
    double calculateTotalCost() const {
        return unitPrice * quantity;
    }
};

enum SortCriterion {
    BY_STORE_NAME,
    BY_TOTAL_COST
};

struct TreeNode {
    Product data;
    TreeNode* left;
    TreeNode* right;

    TreeNode(const Product& p) : data(p), left(nullptr), right(nullptr) {}
};

// --- ÄÎÏÎÌ²ÆÍ² ÔÓÍÊÖ²¯ ÂÂÅÄÅÍÍß (ÊÎÍÒĞÎËÜ ÏÎÌÈËÎÊ) ---

int getValidInt(const string& prompt) {
    int val;
    while (true) {
        cout << prompt;
        if (cin >> val && val >= 0) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return val;
        }
        cout << "Error: please enter a correct positive integer.\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}

double getValidDouble(const string& prompt) {
    double val;
    while (true) {
        cout << prompt;
        if (cin >> val && val >= 0.0) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return val;
        }
        cout << "Error: please enter a correct positive number.\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}

string getValidString(const string& prompt) {
    string val;
    while (true) {
        cout << prompt;
        getline(cin, val);
        if (!val.empty()) {
            return val;
        }
        cout << "Error: string cannot be empty.\n";
    }
}

// --- ËÎÃ²ÊÀ ÄÅĞÅÂÀ ÒÀ ÏÎĞ²ÂÍßÍÜ ---

int compareProducts(const Product& p1, const Product& p2, SortCriterion criterion) {
    if (criterion == BY_STORE_NAME) {
        if (p1.storeName < p2.storeName) return -1;
        if (p1.storeName > p2.storeName) return 1;
        return 0;
    }
    else {
        double cost1 = p1.calculateTotalCost();
        double cost2 = p2.calculateTotalCost();
        if (cost1 < cost2) return -1;
        if (cost1 > cost2) return 1;
        return 0;
    }
}

void insertNode(TreeNode*& root, TreeNode* newNode, SortCriterion criterion) {
    if (!root) {
        root = newNode;
        return;
    }
    if (compareProducts(newNode->data, root->data, criterion) < 0) {
        insertNode(root->left, newNode, criterion);
    }
    else {
        insertNode(root->right, newNode, criterion);
    }
}

// Ôóíêö³ÿ ïîøóêó âêàç³âíèêà íà âóçîë çà íàçâîş (ïîâåğòàº âêàç³âíèê íà âêàç³âíèê)
TreeNode** findNodePtrByName(TreeNode** rootPtr, const string& productName) {
    if (!(*rootPtr)) return nullptr;
    if ((*rootPtr)->data.name == productName) return rootPtr;

    TreeNode** leftResult = findNodePtrByName(&((*rootPtr)->left), productName);
    if (leftResult) return leftResult;

    return findNodePtrByName(&((*rootPtr)->right), productName);
}

bool deleteNodeByName(TreeNode*& root, const string& productName) {
    TreeNode** nodePtr = findNodePtrByName(&root, productName);
    if (!nodePtr) return false;

    TreeNode* toDelete = *nodePtr;
    if (!toDelete->left) {
        *nodePtr = toDelete->right;
    }
    else if (!toDelete->right) {
        *nodePtr = toDelete->left;
    }
    else {
        TreeNode** minPtr = &(toDelete->right);
        while ((*minPtr)->left) {
            minPtr = &((*minPtr)->left);
        }
        toDelete->data = (*minPtr)->data;
        TreeNode* minNode = *minPtr;
        *minPtr = minNode->right;
        delete minNode;
        return true;
    }
    delete toDelete;
    return true;
}

// Ïåğåáóäîâà äåğåâà äëÿ çì³íè ñîğòóâàííÿ
void rebuildTreeRecursive(TreeNode* oldNode, TreeNode*& newRoot, SortCriterion crit) {
    if (!oldNode) return;
    TreeNode* leftNode = oldNode->left;
    TreeNode* rightNode = oldNode->right;

    oldNode->left = nullptr;
    oldNode->right = nullptr;
    insertNode(newRoot, oldNode, crit);

    rebuildTreeRecursive(leftNode, newRoot, crit);
    rebuildTreeRecursive(rightNode, newRoot, crit);
}

void sortTree(TreeNode*& root, SortCriterion newCrit, SortCriterion& currentCrit) {
    if (!root || newCrit == currentCrit) return;
    TreeNode* newRoot = nullptr;
    rebuildTreeRecursive(root, newRoot, newCrit);
    root = newRoot;
    currentCrit = newCrit;
}

// --- ÁÀËÀÍÑÓÂÀÍÍß (DSW ÀËÃÎĞÈÒÌ) ---

int createVine(TreeNode* dummyRoot) {
    int count = 0;
    TreeNode* tail = dummyRoot;
    TreeNode* rest = tail->right;
    while (rest != nullptr) {
        if (rest->left == nullptr) {
            tail = rest;
            rest = rest->right;
            count++;
        }
        else {
            TreeNode* temp = rest->left;
            rest->left = temp->right;
            temp->right = rest;
            rest = temp;
            tail->right = temp;
        }
    }
    return count;
}

void compressTree(TreeNode* dummyRoot, int count) {
    TreeNode* scanner = dummyRoot;
    for (int i = 0; i < count; i++) {
        TreeNode* child = scanner->right;
        scanner->right = child->right;
        scanner = scanner->right;
        child->right = scanner->left;
        scanner->left = child;
    }
}

void balanceTree(TreeNode*& root) {
    if (!root) return;

    TreeNode dummy{ Product() };
    dummy.right = root;

    int count = createVine(&dummy);
    int h = floor(log2(count + 1));
    int expectedLeaves = count + 1 - pow(2, h);

    compressTree(&dummy, expectedLeaves);
    count = count - expectedLeaves;

    while (count > 1) {
        compressTree(&dummy, count / 2);
        count /= 2;
    }
    root = dummy.right;
}

// --- ÔÓÍÊÖ²¯ ÂÈÂÎÄÓ ÒÀ ÔÀÉËÎÂÈÕ ÎÏÅĞÀÖ²É ---

void printProductHeader() {
    cout << left << setw(20) << "Product Name"
        << setw(20) << "Store"
        << setw(15) << "Price/Unit"
        << setw(10) << "Qty"
        << setw(15) << "Unit Type"
        << setw(15) << "Total Cost" << endl;
    cout << string(95, '-') << endl;
}

void printProductNode(TreeNode* node) {
    cout << left << setw(20) << node->data.name
        << setw(20) << node->data.storeName
        << setw(15) << node->data.unitPrice
        << setw(10) << node->data.quantity
        << setw(15) << node->data.unit
        << setw(15) << node->data.calculateTotalCost() << endl;
}

void traverseAndPrintAll(TreeNode* root) {
    if (!root) return;
    traverseAndPrintAll(root->left);
    printProductNode(root);
    traverseAndPrintAll(root->right);
}

void traverseAndPrintByStore(TreeNode* root, const string& targetStore) {
    if (!root) return;
    traverseAndPrintByStore(root->left, targetStore);
    if (root->data.storeName == targetStore) {
        printProductNode(root);
    }
    traverseAndPrintByStore(root->right, targetStore);
}

void traverseAndPrintByPriceRange(TreeNode* root, double minPrice, double maxPrice) {
    if (!root) return;
    traverseAndPrintByPriceRange(root->left, minPrice, maxPrice);
    if (root->data.unitPrice >= minPrice && root->data.unitPrice <= maxPrice) {
        printProductNode(root);
    }
    traverseAndPrintByPriceRange(root->right, minPrice, maxPrice);
}

void freeTreeMemory(TreeNode*& root) {
    if (!root) return;
    freeTreeMemory(root->left);
    freeTreeMemory(root->right);
    delete root;
    root = nullptr;
}

void saveTreeRecursive(TreeNode* root, ofstream& out) {
    if (!root) return;
    out << root->data.name << '\n'
        << root->data.storeName << '\n'
        << root->data.unitPrice << '\n'
        << root->data.quantity << '\n'
        << root->data.unit << '\n';
    saveTreeRecursive(root->left, out);
    saveTreeRecursive(root->right, out);
}

bool saveToFile(TreeNode* root, const string& filename) {
    ofstream out(filename);
    if (!out) return false;
    saveTreeRecursive(root, out);
    return true;
}

bool loadFromFile(TreeNode*& root, SortCriterion criterion, const string& filename) {
    ifstream in(filename);
    if (!in) return false;

    freeTreeMemory(root);
    Product p;
    while (getline(in, p.name)) {
        getline(in, p.storeName);
        in >> p.unitPrice >> p.quantity;
        in.ignore();
        getline(in, p.unit);
        insertNode(root, new TreeNode(p), criterion);
    }
    return true;
}

// --- ²ÍÒÅĞÔÅÉÑ / ÌÅÍŞ ---

Product getProductInputFromUser() {
    Product p;
    p.name = getValidString("Enter product name: ");
    p.storeName = getValidString("Enter store name: ");
    p.unitPrice = getValidDouble("Enter unit price (UAH): ");
    p.quantity = getValidInt("Enter quantity: ");
    p.unit = getValidString("Enter measurement unit (e.g., 'pack of 20kg'): ");
    return p;
}

void displayMenu() {
    cout << "\n--- MAIN MENU ---" << endl;
    cout << "1. Add product" << endl;
    cout << "2. Delete product by name" << endl;
    cout << "3. Edit product" << endl;
    cout << "4. Change sorting (Store Name / Total Cost)" << endl;
    cout << "5. Balance tree (DSW)" << endl;
    cout << "6. Display all products" << endl;
    cout << "7. Display products by store name" << endl;
    cout << "8. Display products by unit price range" << endl;
    cout << "9. Save to file" << endl;
    cout << "10. Load from file" << endl;
    cout << "0. Exit" << endl;
}

int main() {
    TreeNode* root = nullptr;
    SortCriterion currentCriterion = BY_STORE_NAME;
    int choice = -1;

    cout << "Inventory Management System (Binary Search Tree)" << endl;

    while (choice != 0) {
        displayMenu();
        choice = getValidInt("Select an option: ");

        switch (choice) {
        case 1: {
            Product newProduct = getProductInputFromUser();
            insertNode(root, new TreeNode(newProduct), currentCriterion);
            cout << "Product added successfully!\n";
            break;
        }
        case 2: {
            string name = getValidString("Enter product name to delete: ");
            if (deleteNodeByName(root, name)) {
                cout << "Product deleted.\n";
            }
            else {
                cout << "Product not found.\n";
            }
            break;
        }
        case 3: {
            string name = getValidString("Enter product name to edit: ");
            TreeNode** nodePtr = findNodePtrByName(&root, name);
            if (nodePtr) {
                cout << "Enter new data for the product:\n";
                Product updated = getProductInputFromUser();
                deleteNodeByName(root, name);
                insertNode(root, new TreeNode(updated), currentCriterion);
                cout << "Product updated.\n";
            }
            else {
                cout << "Product not found.\n";
            }
            break;
        }
        case 4: {
            int sortChoice = getValidInt("1 - By Store Name, 2 - By Total Cost: ");
            SortCriterion newCrit = (sortChoice == 2) ? BY_TOTAL_COST : BY_STORE_NAME;
            sortTree(root, newCrit, currentCriterion);
            cout << "Tree re-sorted!\n";
            break;
        }
        case 5: {
            balanceTree(root);
            cout << "Tree successfully balanced (DSW)!\n";
            break;
        }
        case 6: {
            if (!root) cout << "The tree is empty.\n";
            else {
                printProductHeader();
                traverseAndPrintAll(root);
            }
            break;
        }
        case 7: {
            string store = getValidString("Enter store name: ");
            printProductHeader();
            traverseAndPrintByStore(root, store);
            break;
        }
        case 8: {
            double minP = getValidDouble("Enter minimum price: ");
            double maxP = getValidDouble("Enter maximum price: ");
            printProductHeader();
            traverseAndPrintByPriceRange(root, minP, maxP);
            break;
        }
        case 9: {
            string file = getValidString("Enter filename to save: ");
            if (saveToFile(root, file)) cout << "Saved successfully.\n";
            else cout << "Error saving file.\n";
            break;
        }
        case 10: {
            string file = getValidString("Enter filename to load: ");
            if (loadFromFile(root, currentCriterion, file)) cout << "Loaded successfully.\n";
            else cout << "Error loading file.\n";
            break;
        }
        case 0:
            cout << "Exiting program...\n";
            break;
        default:
            cout << "Invalid selection.\n";
        }
    }

    freeTreeMemory(root);
    return 0;
}