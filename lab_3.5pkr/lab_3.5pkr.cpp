#include <iostream>
#include <fstream>
#include <string>
#include <cctype>

// Structure for a Stack node (based on a linked list)
struct Node {
    char data;
    Node* next;
};

// 1. Function to push an element onto the stack
void push(Node*& top, char value) {
    Node* newNode = new Node;
    newNode->data = value;
    // STACK LOGIC: New node points to the current top (building upwards)
    newNode->next = top;
    // STACK LOGIC: The pointer to the stack head now points to the new element
    top = newNode;
}

// 2. Function to pop an element from the stack
char pop(Node*& top) {
    // STACK LOGIC: We can only remove the element that is at the very top
    if (top == nullptr) return '\0';
    Node* temp = top;
    char value = temp->data;
    // STACK LOGIC: Move the top pointer to the next element down the stack
    top = top->next;
    delete temp;
    return value;
}

// 3. Function to print stack elements
void printStack(Node* top) {
    if (top == nullptr) {
        std::cout << "Stack is empty." << std::endl;
        return;
    }
    Node* temp = top;
    // STACK LOGIC: Traversal always starts from the TOP
    std::cout << "Stack (top to bottom): ";
    while (temp != nullptr) {
        std::cout << "|" << temp->data << "| ";
        temp = temp->next;
    }
    std::cout << std::endl;
}

// 4. Function to check if a character is an operator
bool isOperator(char c) {
    return (c == '+' || c == '-' || c == '*' || c == '/');
}

// 5. Function to analyze the expression using a stack
void analyzeExpression(std::string expr, int& operands, int& operators) {
    Node* stack = nullptr; // Initializing an empty stack
    operands = 0;
    operators = 0;

    // First, push all relevant characters to the stack
    for (char c : expr) {
        if (!isspace(c)) {
            // STACK LOGIC: Adding symbols to the top of the stack
            push(stack, c);
        }
    }

    std::cout << "Current expression symbols added to ";
    printStack(stack);

    // Process the stack
    while (stack != nullptr) {
        // STACK LOGIC: Extracting elements in REVERSE order (Last In - First Out)
        char current = pop(stack);
        if (isdigit(current)) {
            operands++;
            // Skip consecutive digits to count a multi-digit number as one operand
            // Note: simplistic approach for basic arithmetic strings
        }
        else if (isOperator(current)) {
            operators++;
        }
    }
}

// 6. Function to clear stack memory
void clearStack(Node*& top) {
    while (top != nullptr) {
        // STACK LOGIC: Systematic removal from the top until empty
        pop(top);
    }
}

int main() {
    std::ifstream file("input.txt");
    if (!file.is_open()) {
        std::cerr << "Error: Could not open input.txt" << std::endl;
        // Create a dummy file for demonstration if not exists
        std::ofstream outfile("input.txt");
        outfile << "12 + 5 * 3 / 2";
        outfile.close();
        file.open("input.txt");
    }

    std::string line;
    int totalOperands = 0;
    int totalOperators = 0;

    while (getline(file, line)) {
        int currentOps, currentVars;
        std::cout << "\nProcessing expression: " << line << std::endl;

        analyzeExpression(line, currentVars, currentOps);

        totalOperands += currentVars;
        totalOperators += currentOps;
    }

    std::cout << "\n--- FINAL RESULTS ---" << std::endl;
    std::cout << "Total Operands found: " << totalOperands << std::endl;
    std::cout << "Total Operators found: " << totalOperators << std::endl;

    file.close();
    return 0;
}