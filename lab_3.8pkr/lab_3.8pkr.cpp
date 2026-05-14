#include <iostream>
#include <string>
#include <cmath>
#include <utility>

using namespace std;

// Структура вузла кільцевого двонаправленого списку
struct Node {
    string cityName;
    double distanceToNext; // Відстань до наступного населеного пункту
    Node* next;
    Node* prev;
};

// Функція для додавання нового населеного пункту
void appendSettlement(Node*& head, const string& name, double distance) {
    Node* newNode = new Node{ name, distance, nullptr, nullptr };

    if (head == nullptr) {
        // [CIRCULAR]: Перший елемент вказує сам на себе в обох напрямках
        newNode->next = newNode;
        newNode->prev = newNode;
        head = newNode;
    }
    else {
        // [CIRCULAR]: В кільцевому списку head->prev — це завжди останній елемент (tail)
        Node* tail = head->prev;

        tail->next = newNode;
        newNode->prev = tail;

        // [CIRCULAR]: Новий вузол замикає кільце, вказуючи на head
        newNode->next = head;
        head->prev = newNode;
    }
}

// Функція для виведення автотраси на екран
void printHighway(Node* head) {
    if (head == nullptr) {
        cout << "Highway is empty." << endl;
        return;
    }

    Node* current = head;
    cout << "Highway Scheme (Circular):" << endl;
    do {
        cout << "[" << current->cityName << "] --(" << current->distanceToNext << " km)--> ";
        current = current->next;
        // [CIRCULAR]: Умова зупинки — повернення до початкового елемента head
    } while (current != head);
    cout << "(back to " << head->cityName << ")" << endl;
}

// Допоміжна функція пошуку вузла за назвою
Node* findSettlement(Node* head, const string& name) {
    if (head == nullptr) return nullptr;

    Node* current = head;
    do {
        if (current->cityName == name) {
            return current;
        }
        current = current->next;
    } while (current != head); // [CIRCULAR]: Обхід по колу

    return nullptr;
}

// Функція, яка обчислює та повертає найкоротшу відстань між двома пунктами
double calculateDistanceBetween(Node* head, const string& name1, const string& name2) {
    Node* node1 = findSettlement(head, name1);
    Node* node2 = findSettlement(head, name2);

    if (node1 == nullptr || node2 == nullptr) {
        return -1.0;
    }

    if (node1 == node2) return 0.0;

    // 1. [CIRCULAR]: Розрахунок відстані "вперед" через next
    double forwardDistance = 0.0;
    Node* current = node1;
    while (current != node2) {
        forwardDistance += current->distanceToNext;
        current = current->next;
    }

    // 2. [CIRCULAR]: Розрахунок відстані "назад" через prev
    double backwardDistance = 0.0;
    current = node1;
    while (current != node2) {
        backwardDistance += current->prev->distanceToNext;
        current = current->prev;
    }

    // Повертаємо найменшу з двох відстаней (логіка кільця)
    return min(forwardDistance, backwardDistance);
}

// Функція для пошуку пари міст з найближчою до заданої відстанню
pair<string, string> findClosestDistancePair(Node* head, double targetDistance) {
    pair<string, string> result = { "", "" };

    if (head == nullptr || head->next == head) {
        return result;
    }

    double minDifference = -1.0;
    Node* current1 = head;

    do {
        Node* current2 = current1->next;
        while (current2 != head) {
            double currentDist = calculateDistanceBetween(head, current1->cityName, current2->cityName);
            double diff = abs(currentDist - targetDistance);

            if (minDifference < 0 || diff < minDifference) {
                minDifference = diff;
                result = { current1->cityName, current2->cityName };
            }
            current2 = current2->next;
        }
        current1 = current1->next;
    } while (current1 != head->prev && current1 != head);

    return result;
}

// Функція для звільнення пам'яті
void clearHighway(Node*& head) {
    if (head == nullptr) return;

    Node* current = head;
    Node* tail = head->prev;

    // [CIRCULAR]: Розриваємо кільце перед видаленням, щоб уникнути зациклення
    tail->next = nullptr;

    while (current != nullptr) {
        Node* nextNode = current->next;
        delete current;
        current = nextNode;
    }
    head = nullptr;
}

int main() {
    Node* highway = nullptr;

    // Формування списку
    appendSettlement(highway, "Lviv", 130.5);
    appendSettlement(highway, "Ternopil", 110.0);
    appendSettlement(highway, "Khmelnytskyi", 120.0);
    appendSettlement(highway, "Vinnytsia", 360.5);

    // Виведення списку
    printHighway(highway);
    cout << "------------------------------------------------" << endl;

    // Завдання 1: Відстань між заданими пунктами
    string cityA = "Lviv";
    string cityB = "Vinnytsia";
    double dist = calculateDistanceBetween(highway, cityA, cityB);

    if (dist >= 0) {
        cout << "Distance between " << cityA << " and " << cityB << " is: " << dist << " km." << endl;
    }
    else {
        cout << "Error: One or both cities not found." << endl;
    }
    cout << "------------------------------------------------" << endl;

    // Завдання 2: Пошук пари за заданою відстанню
    double targetDist = 240.0;
    pair<string, string> closestPair = findClosestDistancePair(highway, targetDist);

    cout << "Target search distance: " << targetDist << " km." << endl;
    if (!closestPair.first.empty()) {
        double actualDist = calculateDistanceBetween(highway, closestPair.first, closestPair.second);
        cout << "Cities with the closest distance: " << closestPair.first << " and " << closestPair.second << "." << endl;
        cout << "Actual distance between them: " << actualDist << " km." << endl;
    }
    else {
        cout << "Could not find a valid pair of cities." << endl;
    }
    cout << "------------------------------------------------" << endl;

    clearHighway(highway);

    return 0;
}