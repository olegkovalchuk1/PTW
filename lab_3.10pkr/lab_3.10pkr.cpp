#include <iostream>
#include <string>


struct Node {
    int val;        // Основне поле (ціле число)
    char id;        // Допоміжне поле (символ)
    Node* left;
    Node* right;
};

// ---------------------------------------------------------
// Допоміжні функції для роботи з деревом
// ---------------------------------------------------------

// Рекурсивне обчислення кількості вузлів для підтримки структурного балансу
int countNodes(Node* root) {
    if (!root) return 0;
    return 1 + countNodes(root->left) + countNodes(root->right);
}

// 1. Вставка в звичайне бінарне дерево (заповнюємо дерево рівномірно)
void insertNode(Node*& root, int val, char id) {
    if (!root) {
        root = new Node{ val, id, nullptr, nullptr };
        return;
    }
    // Обираємо гілку, де менше елементів, щоб дерево не перекошувалось
    if (countNodes(root->left) <= countNodes(root->right)) {
        insertNode(root->left, val, id);
    }
    else {
        insertNode(root->right, val, id);
    }
}

// Створення дерева з N елементів (введення з клавіатури)
void createTree(Node*& root, int count) {
    for (int i = 0; i < count; ++i) {
        int v;
        char c;
        std::cout << "Enter int and char for node " << i + 1 << ": ";
        std::cin >> v >> c;
        insertNode(root, v, c);
    }
}

// 2. Вивід структури дерева на екран (рекурсивний обхід)
void printTree(Node* root, int space = 0) {
    if (!root) return;
    space += 8;
    printTree(root->right, space);
    std::cout << std::string(space - 8, ' ') << "-> " << root->val << "(" << root->id << ")\n";
    printTree(root->left, space);
}

// ---------------------------------------------------------
// Видалення елементів (вимоги 6, 7, 8, 9)
// ---------------------------------------------------------

// Рекурсивне очищення пам'яті (видалення всієї гілки)
void clearSubtree(Node*& root) {
    if (!root) return;
    clearSubtree(root->left);
    clearSubtree(root->right);
    delete root;
    root = nullptr;
}

// Пошук найглибшого вузла для заміни при видаленні вузла з двома дітьми
void findDeepest(Node* root, int level, int& maxLevel, Node*& deepest) {
    if (!root) return;
    if (level > maxLevel) {
        maxLevel = level;
        deepest = root;
    }
    findDeepest(root->left, level + 1, maxLevel, deepest);
    findDeepest(root->right, level + 1, maxLevel, deepest);
}

// Видалення конкретного вузла за вказівником (допоміжна для видалення з 2 дітьми)
bool deleteByPointer(Node*& root, Node* target) {
    if (!root) return false;
    if (root == target) {
        delete root;
        root = nullptr;
        return true;
    }
    return deleteByPointer(root->left, target) || deleteByPointer(root->right, target);
}

// 6. Видалення листка (без дітей)
bool deleteLeaf(Node*& root, int val) {
    if (!root) return false;
    if (root->val == val && !root->left && !root->right) {
        delete root; root = nullptr; return true;
    }
    return deleteLeaf(root->left, val) || deleteLeaf(root->right, val);
}

// 7. Видалення вузла з одним нащадком
bool deleteOneChild(Node*& root, int val) {
    if (!root) return false;
    if (root->val == val) {
        if ((root->left && !root->right) || (!root->left && root->right)) {
            Node* temp = root;
            root = (root->left) ? root->left : root->right;
            delete temp;
            return true;
        }
    }
    return deleteOneChild(root->left, val) || deleteOneChild(root->right, val);
}

// 8. Видалення вузла з двома нащадками (через заміну на найглибший вузол)
bool deleteTwoChildren(Node*& root, int val) {
    if (!root) return false;
    if (root->val == val && root->left && root->right) {
        int maxL = -1;
        Node* deepest = nullptr;
        findDeepest(root, 0, maxL, deepest);
        root->val = deepest->val;
        root->id = deepest->id;
        return deleteByPointer(root, deepest);
    }
    return deleteTwoChildren(root->left, val) || deleteTwoChildren(root->right, val);
}

// 9. Видалення цілої гілки
bool deleteBranch(Node*& root, int val) {
    if (!root) return false;
    if (root->val == val) {
        clearSubtree(root);
        return true;
    }
    return deleteBranch(root->left, val) || deleteBranch(root->right, val);
}

// ---------------------------------------------------------
// ВАРІАНТ 9: Спеціальне опрацювання
// ---------------------------------------------------------
// Пошук останнього вузла з мінімальним значенням (Префіксний обхід: Корінь -> Л -> П)
void findLastMinPreOrder(Node* root, Node*& lastMinNode, int& currentMin) {
    if (!root) return;

    // Умова <= забезпечує знаходження саме ОСТАННЬОГО за порядком обходу мінімуму
    if (lastMinNode == nullptr || root->val <= currentMin) {
        currentMin = root->val;
        lastMinNode = root;
    }

    findLastMinPreOrder(root->left, lastMinNode, currentMin);
    findLastMinPreOrder(root->right, lastMinNode, currentMin);
}

// ---------------------------------------------------------
// Меню програми
// ---------------------------------------------------------
int main() {
    Node* root = nullptr;
    int choice;

    do {
        std::cout << "\n--- BINARY TREE MENU ---\n";
        std::cout << "1. Create tree with N elements\n";
        std::cout << "2. Print tree structure\n";
        std::cout << "3. Add new element\n";
        std::cout << "4. Rebalance tree (Rebuild)\n";
        std::cout << "5. Delete leaf (0 children)\n";
        std::cout << "6. Delete node with 1 child\n";
        std::cout << "7. Delete node with 2 children\n";
        std::cout << "8. Delete branch (all descendants)\n";
        std::cout << "9. TASK 9: Find last min element (Pre-order)\n";
        std::cout << "0. Exit\n";
        std::cout << "Choice: ";
        std::cin >> choice;

        int v, n;
        char c;

        switch (choice) {
        case 1:
            clearSubtree(root);
            std::cout << "Count of elements: "; std::cin >> n;
            createTree(root, n);
            break;
        case 2:
            if (!root) std::cout << "Tree is empty.\n";
            else printTree(root);
            break;
        case 3:
            std::cout << "Enter val and char: "; std::cin >> v >> c;
            insertNode(root, v, c);
            break;
        case 4: {
            // Балансування для звичайного дерева — це просто перевставка вузлів
            Node* newRoot = nullptr;
            auto reinsert = [&](auto self, Node* node) -> void {
                if (!node) return;
                insertNode(newRoot, node->val, node->id);
                self(self, node->left);
                self(self, node->right);
                };
            reinsert(reinsert, root);
            clearSubtree(root);
            root = newRoot;
            std::cout << "Tree rebuilt (balanced).\n";
            break;
        }
        case 5:
            std::cout << "Value to delete: "; std::cin >> v;
            std::cout << (deleteLeaf(root, v) ? "Success\n" : "Failed\n");
            break;
        case 6:
            std::cout << "Value to delete: "; std::cin >> v;
            std::cout << (deleteOneChild(root, v) ? "Success\n" : "Failed\n");
            break;
        case 7:
            std::cout << "Value to delete: "; std::cin >> v;
            std::cout << (deleteTwoChildren(root, v) ? "Success\n" : "Failed\n");
            break;
        case 8:
            std::cout << "Branch root value: "; std::cin >> v;
            std::cout << (deleteBranch(root, v) ? "Success\n" : "Failed\n");
            break;
        case 9: {
            if (!root) { std::cout << "Empty\n"; break; }
            Node* resultNode = nullptr;
            int minVal = 0;
            findLastMinPreOrder(root, resultNode, minVal);
            if (resultNode) {
                std::cout << "LAST MIN FOUND:\nValue: " << resultNode->val
                    << "\nChar: " << resultNode->id << "\n";
            }
            break;
        }
        }
    } while (choice != 0);

    clearSubtree(root);
    return 0;
}