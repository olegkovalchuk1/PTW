#include <iostream>

// Structure for a doubly linked list node
struct Node {
    int data;
    Node* next;
    Node* prev;
};

// Function to create a new node
Node* createNode(int value) {
    Node* newNode = new Node;
    newNode->data = value;
    newNode->next = nullptr;
    newNode->prev = nullptr;
    return newNode;
}

// Function to add a node to the end of the doubly linked list
void appendNode(Node*& head, int value) {
    Node* newNode = createNode(value);
    if (head == nullptr) {
        head = newNode;
    }
    else {
        Node* temp = head;
        while (temp->next != nullptr) {
            temp = temp->next;
        }
        temp->next = newNode;
        newNode->prev = temp;
    }
}

// Function to print the list from head to tail
void printList(Node* head) {
    if (head == nullptr) {
        std::cout << "List is empty." << std::endl;
        return;
    }
    Node* temp = head;
    while (temp != nullptr) {
        std::cout << "[" << temp->data << "] <-> ";
        temp = temp->next;
    }
    std::cout << "NULL" << std::endl;
}

// Function that checks if the list contains at least one pair of identical elements
// Returns true if a pair is found, false otherwise
bool hasDuplicateElements(Node* head) {
    if (head == nullptr) return false;

    Node* current = head;
    while (current != nullptr) {
        Node* runner = current->next;
        while (runner != nullptr) {
            if (current->data == runner->data) {
                return true; // Pair found
            }
            runner = runner->next;
        }
        current = current->next;
    }
    return false;
}

// Function to free the memory allocated for the list
void clearList(Node*& head) {
    while (head != nullptr) {
        Node* temp = head;
        head = head->next;
        delete temp;
    }
}

int main() {
    Node* myList = nullptr;
    int count, val;

    std::cout << "Enter the number of elements: ";
    if (!(std::cin >> count)) return 0;

    for (int i = 0; i < count; ++i) {
        std::cout << "Enter value for node " << i + 1 << ": ";
        std::cin >> val;
        appendNode(myList, val);
    }

    std::cout << "\nCreated doubly linked list:" << std::endl;
    printList(myList);

    // Performing the task: searching for duplicates
    bool result = hasDuplicateElements(myList);

    std::cout << "\nTask result:" << std::endl;
    if (result) {
        std::cout << "The list contains at least one pair of identical elements." << std::endl;
    }
    else {
        std::cout << "No duplicate elements found in the list." << std::endl;
    }

    clearList(myList);
    return 0;
}