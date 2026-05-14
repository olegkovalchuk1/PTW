#include <iostream>
#include <string>
#include <fstream>
#include <limits>
#include <algorithm>

using namespace std;

const int DISK_TOTAL_SIZE = 368640;
const int MIN_FILE_SIZE = 18;
const int MAX_FILE_SIZE = 32768;

// Вузли дерев
struct OccupiedNode {
    string name;
    int start;
    int size;
    int height;
    OccupiedNode* left;
    OccupiedNode* right;

    OccupiedNode(string n, int st, int sz)
        : name(n), start(st), size(sz), height(1), left(nullptr), right(nullptr) {
    }
};

struct FreeNode {
    int start;
    int size;
    int height;
    FreeNode* left;
    FreeNode* right;

    FreeNode(int st, int sz)
        : start(st), size(sz), height(1), left(nullptr), right(nullptr) {
    }
};

// АВЛ Дерево зайнятих ділянок
class OccupiedTree {
private:
    OccupiedNode* root;

    int getHeight(OccupiedNode* node) { return node ? node->height : 0; }
    int getBalanceFactor(OccupiedNode* node) { return node ? getHeight(node->right) - getHeight(node->left) : 0; }

    void updateHeight(OccupiedNode* node) {
        if (node) node->height = 1 + max(getHeight(node->left), getHeight(node->right));
    }

    OccupiedNode* rotateRight(OccupiedNode* y) {
        OccupiedNode* x = y->left;
        OccupiedNode* T2 = x->right;
        x->right = y;
        y->left = T2;
        updateHeight(y);
        updateHeight(x);
        return x;
    }

    OccupiedNode* rotateLeft(OccupiedNode* x) {
        OccupiedNode* y = x->right;
        OccupiedNode* T2 = y->left;
        y->left = x;
        x->right = T2;
        updateHeight(x);
        updateHeight(y);
        return y;
    }

    OccupiedNode* balance(OccupiedNode* node) {
        updateHeight(node);
        int bf = getBalanceFactor(node);

        if (bf > 1) {
            if (getBalanceFactor(node->right) < 0) node->right = rotateRight(node->right);
            return rotateLeft(node);
        }
        if (bf < -1) {
            if (getBalanceFactor(node->left) > 0) node->left = rotateLeft(node->left);
            return rotateRight(node);
        }
        return node;
    }

    OccupiedNode* insertNode(OccupiedNode* node, string name, int start, int size) {
        if (!node) return new OccupiedNode(name, start, size);
        if (name < node->name) node->left = insertNode(node->left, name, start, size);
        else if (name > node->name) node->right = insertNode(node->right, name, start, size);
        else return node;
        return balance(node);
    }

    OccupiedNode* findMin(OccupiedNode* node) {
        while (node->left) node = node->left;
        return node;
    }

    OccupiedNode* removeNode(OccupiedNode* node, string name, int& outStart, int& outSize, bool& found) {
        if (!node) return nullptr;

        if (name < node->name) node->left = removeNode(node->left, name, outStart, outSize, found);
        else if (name > node->name) node->right = removeNode(node->right, name, outStart, outSize, found);
        else {
            found = true;
            outStart = node->start;
            outSize = node->size;

            if (!node->left || !node->right) {
                OccupiedNode* temp = node->left ? node->left : node->right;
                if (!temp) { temp = node; node = nullptr; }
                else *node = *temp;
                delete temp;
            }
            else {
                OccupiedNode* temp = findMin(node->right);
                node->name = temp->name;
                node->start = temp->start;
                node->size = temp->size;
                bool dummy;
                node->right = removeNode(node->right, temp->name, outStart, outSize, dummy);
            }
        }
        if (!node) return nullptr;
        return balance(node);
    }

    void destroyTree(OccupiedNode* node) {
        if (node) {
            destroyTree(node->left);
            destroyTree(node->right);
            delete node;
        }
    }

    OccupiedNode* searchNode(OccupiedNode* node, string name) {
        if (!node || node->name == name) return node;
        if (name < node->name) return searchNode(node->left, name);
        return searchNode(node->right, name);
    }

    void printInOrder(OccupiedNode* node) {
        if (node) {
            printInOrder(node->left);
            cout << "File: " << node->name << " | Start: " << node->start << " | Size: " << node->size << " bytes\n";
            printInOrder(node->right);
        }
    }

    void savePreOrder(OccupiedNode* node, ofstream& out) {
        if (node) {
            out << node->name << " " << node->start << " " << node->size << "\n";
            savePreOrder(node->left, out);
            savePreOrder(node->right, out);
        }
    }

public:
    OccupiedTree() : root(nullptr) {}
    ~OccupiedTree() { clear(); }

    void insert(string name, int start, int size) { root = insertNode(root, name, start, size); }

    bool remove(string name, int& outStart, int& outSize) {
        bool found = false;
        root = removeNode(root, name, outStart, outSize, found);
        return found;
    }

    bool contains(string name) { return searchNode(root, name) != nullptr; }
    void print() { printInOrder(root); }
    void clear() { destroyTree(root); root = nullptr; }
    void save(ofstream& out) { savePreOrder(root, out); }
};

// АВЛ Дерево вільних ділянок
class FreeTree {
private:
    FreeNode* root;

    int getHeight(FreeNode* node) { return node ? node->height : 0; }
    int getBalanceFactor(FreeNode* node) { return node ? getHeight(node->right) - getHeight(node->left) : 0; }

    void updateHeight(FreeNode* node) {
        if (node) node->height = 1 + max(getHeight(node->left), getHeight(node->right));
    }

    FreeNode* rotateRight(FreeNode* y) {
        FreeNode* x = y->left;
        FreeNode* T2 = x->right;
        x->right = y;
        y->left = T2;
        updateHeight(y);
        updateHeight(x);
        return x;
    }

    FreeNode* rotateLeft(FreeNode* x) {
        FreeNode* y = x->right;
        FreeNode* T2 = y->left;
        y->left = x;
        x->right = T2;
        updateHeight(x);
        updateHeight(y);
        return y;
    }

    FreeNode* balance(FreeNode* node) {
        updateHeight(node);
        int bf = getBalanceFactor(node);

        if (bf > 1) {
            if (getBalanceFactor(node->right) < 0) node->right = rotateRight(node->right);
            return rotateLeft(node);
        }
        if (bf < -1) {
            if (getBalanceFactor(node->left) > 0) node->left = rotateLeft(node->left);
            return rotateRight(node);
        }
        return node;
    }

    FreeNode* insertNode(FreeNode* node, int start, int size) {
        if (!node) return new FreeNode(start, size);
        if (start < node->start) node->left = insertNode(node->left, start, size);
        else if (start > node->start) node->right = insertNode(node->right, start, size);
        else return node;
        return balance(node);
    }

    FreeNode* findMin(FreeNode* node) {
        while (node->left) node = node->left;
        return node;
    }

    FreeNode* removeNode(FreeNode* node, int start, bool& found) {
        if (!node) return nullptr;

        if (start < node->start) node->left = removeNode(node->left, start, found);
        else if (start > node->start) node->right = removeNode(node->right, start, found);
        else {
            found = true;
            if (!node->left || !node->right) {
                FreeNode* temp = node->left ? node->left : node->right;
                if (!temp) { temp = node; node = nullptr; }
                else *node = *temp;
                delete temp;
            }
            else {
                FreeNode* temp = findMin(node->right);
                node->start = temp->start;
                node->size = temp->size;
                bool dummy;
                node->right = removeNode(node->right, temp->start, dummy);
            }
        }
        if (!node) return nullptr;
        return balance(node);
    }

    int getMaxFreeSize(FreeNode* node) {
        if (!node) return 0;
        int leftMax = getMaxFreeSize(node->left);
        int rightMax = getMaxFreeSize(node->right);
        return max({ node->size, leftMax, rightMax });
    }

    bool findFirstFit(FreeNode* node, int reqSize, int& outStart, int& outSize) {
        if (!node) return false;
        if (findFirstFit(node->left, reqSize, outStart, outSize)) return true;
        if (node->size >= reqSize) {
            outStart = node->start;
            outSize = node->size;
            return true;
        }
        return findFirstFit(node->right, reqSize, outStart, outSize);
    }

    void inorderSearchMerge(FreeNode* node, FreeNode*& prev, bool& found, int& s1, int& sz1, int& s2, int& sz2) {
        if (!node || found) return;
        inorderSearchMerge(node->left, prev, found, s1, sz1, s2, sz2);

        if (found) return;

        if (prev != nullptr) {
            if (prev->start + prev->size == node->start) {
                s1 = prev->start; sz1 = prev->size;
                s2 = node->start; sz2 = node->size;
                found = true;
                return;
            }
        }
        prev = node;
        inorderSearchMerge(node->right, prev, found, s1, sz1, s2, sz2);
    }

    void destroyTree(FreeNode* node) {
        if (node) {
            destroyTree(node->left);
            destroyTree(node->right);
            delete node;
        }
    }

    void printInOrder(FreeNode* node) {
        if (node) {
            printInOrder(node->left);
            cout << "Free block | Start: " << node->start << " | Size: " << node->size << " bytes\n";
            printInOrder(node->right);
        }
    }

    void savePreOrder(FreeNode* node, ofstream& out) {
        if (node) {
            out << node->start << " " << node->size << "\n";
            savePreOrder(node->left, out);
            savePreOrder(node->right, out);
        }
    }

public:
    FreeTree() : root(nullptr) {}
    ~FreeTree() { clear(); }

    void insert(int start, int size) { root = insertNode(root, start, size); }

    bool remove(int start) {
        bool found = false;
        root = removeNode(root, start, found);
        return found;
    }

    int calculateMaxFreeSize() { return getMaxFreeSize(root); }

    bool allocateSpace(int reqSize, int& allocStart) {
        int outStart, outSize;
        if (findFirstFit(root, reqSize, outStart, outSize)) {
            remove(outStart);
            allocStart = outStart;
            if (outSize > reqSize) {
                insert(outStart + reqSize, outSize - reqSize);
            }
            return true;
        }
        return false;
    }

    // Злиття ділянок
    void mergeAdjacentBlocks() {
        bool merged;
        do {
            merged = false;
            int s1, sz1, s2, sz2;
            bool found = false;
            FreeNode* prev = nullptr;

            inorderSearchMerge(root, prev, found, s1, sz1, s2, sz2);

            if (found) {
                remove(s1);
                remove(s2);
                insert(s1, sz1 + sz2);
                merged = true;
            }
        } while (merged);
    }

    void print() { printInOrder(root); }
    void clear() { destroyTree(root); root = nullptr; }
    void save(ofstream& out) { savePreOrder(root, out); }
};

// Менеджер диска
class DiskManager {
private:
    OccupiedTree occupiedFiles;
    FreeTree freeSpace;

public:
    DiskManager() {
        freeSpace.insert(0, DISK_TOTAL_SIZE);
    }

    void addFile(string name, int size) {
        if (size < MIN_FILE_SIZE || size > MAX_FILE_SIZE) {
            cout << "[Error] File size must be between " << MIN_FILE_SIZE << " and " << MAX_FILE_SIZE << " bytes.\n";
            return;
        }
        if (occupiedFiles.contains(name)) {
            cout << "[Error] File '" << name << "' already exists.\n";
            return;
        }

        int maxAvailable = freeSpace.calculateMaxFreeSize();
        if (size > maxAvailable) {
            cout << ">>> CRITICAL ERROR <<<\n";
            cout << "File '" << name << "' exceeds the largest free block size (" << maxAvailable << " bytes).\n";
            return;
        }

        int startAddress;
        if (freeSpace.allocateSpace(size, startAddress)) {
            occupiedFiles.insert(name, startAddress, size);
            cout << "[Success] File '" << name << "' successfully written.\n";
        }
        else {
            cout << "[Error] Could not find space for the file.\n";
        }
    }

    void removeFile(string name) {
        int start, size;
        if (occupiedFiles.remove(name, start, size)) {
            freeSpace.insert(start, size);
            freeSpace.mergeAdjacentBlocks();
            cout << "[Success] File '" << name << "' deleted.\n";
        }
        else {
            cout << "[Error] File '" << name << "' not found on disk.\n";
        }
    }

    void printDiskState() {
        cout << "\n=== OCCUPIED BLOCKS (FILES) ===\n";
        occupiedFiles.print();
        cout << "\n=== FREE BLOCKS ===\n";
        freeSpace.print();
        cout << "===============================\n";
    }

    void saveToFile(string filename) {
        if (filename.find(".txt") == string::npos) {
            filename += ".txt";
        }

        ofstream out(filename);
        if (!out) {
            cout << "[Error] Failed to create or open file '" << filename << "'.\n";
            return;
        }
        out << "OCCUPIED\n";
        occupiedFiles.save(out);
        out << "FREE\n";
        freeSpace.save(out);
        out.close();
        cout << "[Success] Disk state saved to file: " << filename << "\n";
        cout << "(Check your IDE project folder for this file!)\n";
    }

    void loadFromFile(string filename) {
        if (filename.find(".txt") == string::npos) {
            filename += ".txt";
        }

        ifstream in(filename);
        if (!in) {
            cout << "[Error] File '" << filename << "' not found.\n";
            cout << "(Ensure it exists in your IDE working directory).\n";
            return;
        }

        occupiedFiles.clear();
        freeSpace.clear();

        string marker;
        in >> marker;
        if (marker == "OCCUPIED") {
            string name;
            int start, size;
            while (in >> name && name != "FREE") {
                in >> start >> size;
                occupiedFiles.insert(name, start, size);
            }
        }

        int start, size;
        while (in >> start >> size) {
            freeSpace.insert(start, size);
        }

        in.close();
        cout << "[Success] Disk state loaded from file: " << filename << "\n";
    }
};

// Функції вводу
void clearInputError() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

int getIntInput(string prompt) {
    int value;
    while (true) {
        cout << prompt;
        if (cin >> value) {
            clearInputError();
            return value;
        }
        cout << "[Error] Please enter a valid number.\n";
        clearInputError();
    }
}

int main() {
    DiskManager disk;
    int choice;

    do {
        cout << "\n--- FLOPPY DISK MANAGER ---\n";
        cout << "1. Write file\n";
        cout << "2. Delete file\n";
        cout << "3. Print disk memory state\n";
        cout << "4. Save structure to file\n";
        cout << "5. Load structure from file\n";
        cout << "0. Exit\n";

        choice = getIntInput("Your choice: ");

        switch (choice) {
        case 1: {
            string name;
            cout << "Enter file name: ";
            cin >> name;
            int size = getIntInput("Enter file size (in bytes, 18 - 32768): ");
            disk.addFile(name, size);
            break;
        }
        case 2: {
            string name;
            cout << "Enter file name to delete: ";
            cin >> name;
            disk.removeFile(name);
            break;
        }
        case 3:
            disk.printDiskState();
            break;
        case 4: {
            string filename;
            cout << "Enter file name to save (e.g., mydisk): ";
            cin >> filename;
            disk.saveToFile(filename);
            break;
        }
        case 5: {
            string filename;
            cout << "Enter file name to load: ";
            cin >> filename;
            disk.loadFromFile(filename);
            break;
        }
        case 0:
            cout << "Exiting.\n";
            break;
        default:
            cout << "[Error] Invalid menu option.\n";
        }
    } while (choice != 0);

    return 0;
}