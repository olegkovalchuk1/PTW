#include <iostream>
#include <string>

// Структура вузла лінійного однонаправленого списку
struct Node {
    int data;
    Node* next;
};

// 1. Функція створення нового вузла
Node* createNode(int value) {
    Node* newNode = new Node;
    newNode->data = value;
    newNode->next = nullptr;
    return newNode;
}

// 2. Функція додавання елемента в кінець списку
void appendNode(Node*& head, int value) {
    if (head == nullptr) {
        head = createNode(value);
    }
    else {
        Node* temp = head;
        while (temp->next != nullptr) {
            temp = temp->next;
        }
        temp->next = createNode(value);
    }
}

// 3. Функція для друку списку
void printList(Node* head) {
    if (head == nullptr) {
        std::cout << "List is empty." << std::endl;
        return;
    }
    Node* temp = head;
    while (temp != nullptr) {
        std::cout << "[" << temp->data << "] -> ";
        temp = temp->next;
    }
    std::cout << "NULL" << std::endl;
}

// 4. Функція вилучення елементів, що передують заданому значенню
// Виконує роль модифікації структури списку
void removePrecedingElements(Node*& head, int target) {
    if (head == nullptr || head->next == nullptr) return;

    // Перевірка, чи є перший елемент таким, що передує цільовому значенню
    while (head != nullptr && head->next != nullptr && head->next->data == target) {
        Node* temp = head;
        head = head->next;
        delete temp;
    }

    // Перевірка решти списку (дивимось на два вузли вперед)
    Node* current = head;
    while (current != nullptr && current->next != nullptr && current->next->next != nullptr) {
        if (current->next->next->data == target) {
            Node* toDelete = current->next;
            current->next = current->next->next;
            delete toDelete;
            // Не переходимо до наступного, бо новий current->next може знову підлягати видаленню
        }
        else {
            current = current->next;
        }
    }
}

// 5. Функція для очищення пам'яті
void clearList(Node*& head) {
    while (head != nullptr) {
        Node* temp = head;
        head = head->next;
        delete temp;
    }
}

int main() {
    Node* myList = nullptr;
    int size, value, targetValue;

    std::cout << "Enter the number of elements: ";
    std::cin >> size;

    for (int i = 0; i < size; ++i) {
        std::cout << "Enter value for element " << i + 1 << ": ";
        std::cin >> value;
        appendNode(myList, value);
    }

    std::cout << "\nInitial list:" << std::endl;
    printList(myList);

    std::cout << "\nEnter target value. Elements preceding this value will be deleted: ";
    std::cin >> targetValue;

    removePrecedingElements(myList, targetValue);

    std::cout << "\nResulting list:" << std::endl;
    printList(myList);

    clearList(myList);
    return 0;
}