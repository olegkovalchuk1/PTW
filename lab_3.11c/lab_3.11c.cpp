#include <iostream>
#include <string>
#include <fstream>
#include <limits>

// ============================================================================
// СТРУКТУРИ ДАНИХ ТА ДОПОМІЖНІ ФУНКЦІЇ
// ============================================================================

// Частина 1. Реалізувати структуру даних у вигляді списку
struct MemoryBlock {
    int start_address;
    int size;
    std::string file_name; // Порожня для вільних ділянок
    MemoryBlock* next = nullptr;
};

// Функція, яка створює новий вузол списку
MemoryBlock* create_memory_node(int start, int size, const std::string& name) {
    MemoryBlock* new_node = new MemoryBlock();
    new_node->start_address = start;
    new_node->size = size;
    new_node->file_name = name;
    new_node->next = nullptr;
    return new_node;
}

// Функція, яка очищає список та звільняє пам'ять комп'ютера
void clear_memory_list(MemoryBlock*& head) {
    while (head != nullptr) {
        MemoryBlock* temp = head;
        head = head->next;
        delete temp;
    }
}

// Функція, яка додає ділянку в список, сортуючи за початковою адресою
void insert_ordered_by_address(MemoryBlock*& head, MemoryBlock* new_node) {
    if (head == nullptr || head->start_address >= new_node->start_address) {
        new_node->next = head;
        head = new_node;
        return;
    }
    MemoryBlock* current = head;
    while (current->next != nullptr && current->next->start_address < new_node->start_address) {
        current = current->next;
    }
    new_node->next = current->next;
    current->next = new_node;
}

// Функція, яка об'єднує суміжні вільні ділянки пам'яті (дефрагментація)
void merge_contiguous_free_blocks(MemoryBlock* head) {
    MemoryBlock* current = head;
    while (current != nullptr && current->next != nullptr) {
        if (current->start_address + current->size == current->next->start_address) {
            MemoryBlock* temp = current->next;
            current->size += temp->size;
            current->next = temp->next;
            delete temp;
        }
        else {
            current = current->next;
        }
    }
}

// ============================================================================
// ЛОГІКА РОБОТИ З ПАМ'ЯТТЮ (ОБЧИСЛЕННЯ ТА ПОШУК)
// ============================================================================

// Функція, яка шукає першу підходящу вільну ділянку (First-Fit)
MemoryBlock* find_suitable_free_block(MemoryBlock* free_head, int size) {
    MemoryBlock* current = free_head;
    while (current != nullptr) {
        if (current->size >= size) {
            return current;
        }
        current = current->next;
    }
    return nullptr;
}

// Функція, яка шукає зайнятий файл за його іменем
MemoryBlock* find_occupied_file(MemoryBlock* occ_head, const std::string& name) {
    MemoryBlock* current = occ_head;
    while (current != nullptr) {
        if (current->file_name == name) {
            return current;
        }
        current = current->next;
    }
    return nullptr;
}

// Функція, яка видаляє вузол із списку зайнятих ділянок за адресою
void remove_node_from_occupied(MemoryBlock*& occ_head, int start_address) {
    if (occ_head == nullptr) return;

    if (occ_head->start_address == start_address) {
        MemoryBlock* temp = occ_head;
        occ_head = occ_head->next;
        delete temp;
        return;
    }

    MemoryBlock* current = occ_head;
    while (current->next != nullptr && current->next->start_address != start_address) {
        current = current->next;
    }

    if (current->next != nullptr) {
        MemoryBlock* temp = current->next;
        current->next = current->next->next;
        delete temp;
    }
}

// В процесі роботи файли або записуються на диск...
bool write_file_to_disk(MemoryBlock*& free_head, MemoryBlock*& occ_head, const std::string& name, int size) {
    // На початку роботи файли записуються підряд один за одним.
    // Після видалення файлу на диску утворюється вільна ділянка пам’яті, і новий файл, що записується 
    // або розміщується на вільній ділянці, або, якщо файл не вміщається у вільну ділянку, розміщується після останнього записаного файлу.
    MemoryBlock* target_free = find_suitable_free_block(free_head, size);

    if (target_free == nullptr) {
        // У випадку, коли файл перевищує довжину найбільшої вільної ділянки, видається аварійне повідомлення
        return false;
    }

    // Створюємо запис у списку зайнятих ділянок
    MemoryBlock* new_occupied = create_memory_node(target_free->start_address, size, name);
    insert_ordered_by_address(occ_head, new_occupied);

    // Оновлюємо ділянку у списку вільних
    target_free->start_address += size;
    target_free->size -= size;

    // Якщо вільна ділянка повністю вичерпалась, видаляємо її вузол
    if (target_free->size == 0) {
        if (free_head == target_free) {
            free_head = free_head->next;
            delete target_free;
        }
        else {
            MemoryBlock* prev = free_head;
            while (prev->next != target_free) {
                prev = prev->next;
            }
            prev->next = target_free->next;
            delete target_free;
        }
    }
    return true;
}

// ...або видаляються з нього
bool delete_file_from_disk(MemoryBlock*& free_head, MemoryBlock*& occ_head, const std::string& name) {
    MemoryBlock* target_file = find_occupied_file(occ_head, name);
    if (target_file == nullptr) {
        return false;
    }

    // Створюємо нову вільну ділянку на місці видаленого файлу
    MemoryBlock* new_free = create_memory_node(target_file->start_address, target_file->size, "");
    insert_ordered_by_address(free_head, new_free);
    merge_contiguous_free_blocks(free_head);

    // Видаляємо файл зі списку зайнятих
    remove_node_from_occupied(occ_head, target_file->start_address);
    return true;
}

// ============================================================================
// ВВЕДЕННЯ / ВИВЕДЕННЯ ТА ВАЛІДАЦІЯ (КОНТРОЛЬ ПОМИЛОК)
// ============================================================================

// Необхідно передбачити контроль помилок користувача при введенні даних
int get_valid_menu_choice() {
    int choice;
    while (true) {
        std::cout << "Your choice: ";
        if (std::cin >> choice) {
            return choice;
        }
        std::cout << "Error! Please enter a valid numerical value.\n";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
}

// Контроль помилок при введенні розміру файлу
int get_valid_file_size() {
    int size;
    while (true) {
        std::cout << "Enter file size in bytes (from 18 to 32768): ";
        if (std::cin >> size) {
            // Файли мають довільну довжину від 18 байт до 32 Кбайт
            if (size >= 18 && size <= 32768) {
                return size;
            }
        }
        std::cout << "Error! Allowed file size is from 18 bytes to 32768 bytes (32 KB).\n";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
}

// Програма має виводити дані про зайняті і вільні ділянки пам’яті на диску
void print_memory_state(const MemoryBlock* free_head, const MemoryBlock* occ_head) {
    std::cout << "\n--- DISK MEMORY STATE ---\n";
    std::cout << "OCCUPIED BLOCKS:\n";
    const MemoryBlock* curr = occ_head;
    if (curr == nullptr) std::cout << "  (no occupied blocks)\n";
    while (curr != nullptr) {
        std::cout << "  File: \"" << curr->file_name << "\" | Address: "
            << curr->start_address << " | Size: " << curr->size << " bytes\n";
        curr = curr->next;
    }

    std::cout << "FREE BLOCKS:\n";
    curr = free_head;
    if (curr == nullptr) std::cout << "  (no free blocks! Disk is full)\n";
    while (curr != nullptr) {
        std::cout << "  Free block | Address: " << curr->start_address
            << " | Size: " << curr->size << " bytes\n";
        curr = curr->next;
    }
    std::cout << "-------------------------\n";
}

// ============================================================================
// РОБОТА З ФАЙЛАМИ СИСТЕМИ (ЗБЕРЕЖЕННЯ / ЗАВАНТАЖЕННЯ)
// ============================================================================

// Передбачити можливість збереження всієї структури даних у файл...
void save_structure_to_file(const MemoryBlock* free_head, const MemoryBlock* occ_head, const std::string& filename) {
    std::ofstream out(filename);
    if (!out) {
        std::cout << "Error opening file for saving!\n";
        return;
    }

    // Рахуємо та записуємо зайняті ділянки
    int occ_count = 0;
    const MemoryBlock* curr = occ_head;
    while (curr != nullptr) { occ_count++; curr = curr->next; }
    out << occ_count << "\n";
    curr = occ_head;
    while (curr != nullptr) {
        out << curr->start_address << " " << curr->size << " " << curr->file_name << "\n";
        curr = curr->next;
    }

    // Рахуємо та записуємо вільні ділянки
    int free_count = 0;
    curr = free_head;
    while (curr != nullptr) { free_count++; curr = curr->next; }
    out << free_count << "\n";
    curr = free_head;
    while (curr != nullptr) {
        out << curr->start_address << " " << curr->size << "\n";
        curr = curr->next;
    }
    std::cout << "Data structure successfully saved to file \"" << filename << "\".\n";
}

// ...та завантаження з файлу
void load_structure_from_file(MemoryBlock*& free_head, MemoryBlock*& occ_head, const std::string& filename) {
    std::ifstream in(filename);
    if (!in) {
        std::cout << "Error! Could not open file \"" << filename << "\" for reading.\n";
        return;
    }

    // Очищаємо поточні списки перед завантаженням нових даних
    clear_memory_list(free_head);
    clear_memory_list(occ_head);

    int occ_count, free_count;

    // Читаємо зайняті ділянки
    if (!(in >> occ_count)) return;
    for (int i = 0; i < occ_count; ++i) {
        int start, size;
        std::string name;
        in >> start >> size >> name;
        MemoryBlock* node = create_memory_node(start, size, name);
        insert_ordered_by_address(occ_head, node);
    }

    // Читаємо вільні ділянки
    if (!(in >> free_count)) return;
    for (int i = 0; i < free_count; ++i) {
        int start, size;
        in >> start >> size;
        MemoryBlock* node = create_memory_node(start, size, "");
        insert_ordered_by_address(free_head, node);
    }
    std::cout << "Data structure successfully loaded from file \"" << filename << "\".\n";
}

// ============================================================================
// ГОЛОВНА ПРОГРАМА ТА МЕНЮ
// ============================================================================

int main() {
    // Загальний об’єм пам’яті на диску 360 Кбайт
    const int TOTAL_DISK_SIZE = 360 * 1024; // 368640 байт

    MemoryBlock* free_blocks_head = create_memory_node(0, TOTAL_DISK_SIZE, "");
    MemoryBlock* occupied_blocks_head = nullptr;
    {
        std::ofstream simple_file("simple.txt");
        if (simple_file) {
            // Кількість зайнятих ділянок = 1
            simple_file << "1\n";
            // Адреса 0, розмір 1000 байт, ім'я "test.txt"
            simple_file << "0 1000 test.txt\n";

            // Кількість вільних ділянок = 1
            simple_file << "1\n";
            // Адреса 1000, розмір 367640 байт (залишок пам'яті диска)
            simple_file << "1000 367640\n";

            simple_file.close();
            std::cout << "[INFO] Simple test file 'simple.txt' created successfully!\n";
        }
    }
    int menu_choice;
    // Програма має містити меню
    do {
        std::cout << "\n=== FLOPPY DISK SIMULATOR MENU ===\n";
        std::cout << "1. Write file to disk\n";
        std::cout << "2. Delete file from disk\n";
        std::cout << "3. Print disk memory state\n";
        std::cout << "4. Save data structure to file\n";
        std::cout << "5. Load data structure from file\n";
        std::cout << "0. Exit program\n";

        menu_choice = get_valid_menu_choice();

        switch (menu_choice) {
        case 1: {
            std::string name;
            std::cout << "Enter file name: ";
            std::cin >> name;

            if (find_occupied_file(occupied_blocks_head, name) != nullptr) {
                std::cout << "Error! A file with this name already exists on the disk.\n";
                break;
            }

            int size = get_valid_file_size();

            if (write_file_to_disk(free_blocks_head, occupied_blocks_head, name, size)) {
                std::cout << "File \"" << name << "\" successfully written.\n";
            }
            else {
                // У випадку, коли файл перевищує довжину найбільшої вільної ділянки, видається аварійне повідомлення
                std::cout << "\n[EMERGENCY MESSAGE]: Out of memory! The file exceeds the size of the largest free block.\n";
            }
            break;
        }
        case 2: {
            std::string name;
            std::cout << "Enter file name to delete: ";
            std::cin >> name;

            if (delete_file_from_disk(free_blocks_head, occupied_blocks_head, name)) {
                std::cout << "File \"" << name << "\" successfully deleted from disk.\n";
            }
            else {
                std::cout << "Error! File named \"" << name << "\" not found.\n";
            }
            break;
        }
        case 3:
            print_memory_state(free_blocks_head, occupied_blocks_head);
            break;
        case 4: {
            std::string filename;
            std::cout << "Enter filename to save structure: ";
            std::cin >> filename;
            save_structure_to_file(free_blocks_head, occupied_blocks_head, filename);
            break;
        }
        case 5: {
            std::string filename;
            std::cout << "Enter filename to load structure: ";
            std::cin >> filename;
            load_structure_from_file(free_blocks_head, occupied_blocks_head, filename);
            break;
        }
        case 0:
            std::cout << "Exiting system.\n";
            break;
        default:
            std::cout << "Error! Invalid menu choice. Please try again.\n";
            break;
        }
    } while (menu_choice != 0);

    // Очищення виділеної динамічної пам'яті перед виходом
    clear_memory_list(free_blocks_head);
    clear_memory_list(occupied_blocks_head);

    return 0;
}