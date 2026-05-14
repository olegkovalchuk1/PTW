#include <iostream>

struct Node {
    int data;
    Node* next;
};

Node* createNode(int value) {
    Node* newNode = new Node;
    newNode->data = value;
    newNode->next = nullptr;
    return newNode;
}

void appendToCircularList(Node*& head, int value) {
    Node* newNode = createNode(value);
    if (head == nullptr) {
        head = newNode;
        newNode->next = head; // !!! ЦЕ РОБИТЬ СПИСОК КІЛЬЦЕВИМ (перший вказує на себе)
    }
    else {
        Node* temp = head;
        // Шукаємо останній елемент, який зараз вказує на head
        while (temp->next != head) {
            temp = temp->next;
        }
        temp->next = newNode; // Попередній останній тепер вказує на новий вузол
        newNode->next = head; // !!! НОВИЙ ВУЗОЛ ЗАМИКАЄ КІЛЬЦЕ, вказуючи на head
    }
}

void printCircularList(Node* head) {
    if (head == nullptr) {
        std::cout << "List is empty." << std::endl;
        return;
    }
    Node* temp = head;
    do {
        std::cout << "[" << temp->data << "] -> ";
        temp = temp->next;
    } while (temp != head); // !!! УМОВА: йдемо по колу, поки знову не зустрінемо head
    std::cout << "(circular link back to head)" << std::endl;
}

int countOddValues(Node* head) {
    if (head == nullptr) return 0;

    int count = 0;
    Node* temp = head;
    do {
        if (temp->data % 2 != 0) {
            count++;
        }
        temp = temp->next;
    } while (temp != head); // !!! ОБХІД ЗУПИНЯЄТЬСЯ, коли завершено повне коло

    return count;
}

void clearCircularList(Node*& head) {
    if (head == nullptr) return;

    // Щоб видалити кільцевий список, треба спочатку "розірвати" кільце
    Node* last = head;
    while (last->next != head) {
        last = last->next;
    }
    last->next = nullptr; // !!! РОЗРИВАЄМО КІЛЬЦЕ (робимо список лінійним для видалення)

    Node* current = head;
    while (current != nullptr) {
        Node* nextNode = current->next;
        delete current;
        current = nextNode;
    }
    head = nullptr;
}

int main() {
    Node* circularList = nullptr;
    int n, val;

    std::cout << "Enter the number of elements: ";
    if (!(std::cin >> n) || n < 0) return 0;

    for (int i = 0; i < n; ++i) {
        std::cout << "Enter value for node " << i + 1 << ": ";
        std::cin >> val;
        appendToCircularList(circularList, val);
    }

    std::cout << "\nCircular list elements:" << std::endl;
    printCircularList(circularList);

    int oddCount = countOddValues(circularList);

    std::cout << "\nResult:" << std::endl;
    std::cout << "Number of elements with odd values: " << oddCount << std::endl;

    clearCircularList(circularList);
    return 0;
}