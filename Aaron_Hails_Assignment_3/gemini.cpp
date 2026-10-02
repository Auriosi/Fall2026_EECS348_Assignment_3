/*
Name: EECS 348 Assignment 3 - Email Priority Queue Max-Heap
Description: Implements an email inbox priority queue using a dynamically resizing MaxHeap.
    Emails are prioritized based on sender hierarchy, arrival date (newest first), and
    arrival order. Supports commands to add emails (EMAIL), view the next email (NEXT),
    read/remove the next email (READ), and display the unread email count (COUNT).
Input: Reads commands via redirection from standard input (stdin).
Output: Standard output displaying the number of unread emails, formatted details of the
    highest-priority email, and error/warning messages for malformed commands.
Collaborators: Gemini, GitHub Copilot
Author: Aaron Hails
Creation: 10/1/2026
Revision: 10/1/2026
Revision History: Initial creation.
    Modifications to improve correctness, execution speed, space efficiency, and maintainability.
*/

/* Standard library headers:
 * - <iostream>: Input/output stream objects (cin, cout, cerr).
 * - <string>: String class for message and attribute manipulation.
 * - <sstream>: String stream processing for comma-delimited parsing and formatting.
 * - <cstddef>: Standard size definitions (size_t). */
#include <iostream>
#include <string>
#include <sstream>
#include <cstddef>

// Written by Github Copilot
/* Sender hierarchy mapping categories to integer priority ranks (5 = highest, 1 = lowest). */
enum class SenderRank {
    BOSS = 5,
    SUBORDINATE = 4,
    PEER = 3,
    IMPORTANT_PERSON = 2,
    OTHER_PERSON = 1
};

// Represents an individual email
// Written by Gemini
class Email {
private:
    /* Email attributes: sender rank, subject line, and individual date components. */
    SenderRank sender;
    std::string subject;
    int year, month, day;

    // Written by Github Copilot
    /* Formats the stored year, month, and day into a hyphen-delimited string (YYYY-M-D). */
    std::string getDateString() const {
        std::ostringstream oss;
        oss << year << "-" << month << "-" << day;
        return oss.str();
    }

public:
    // Written by Gemini
    /* Default constructor initializing fields to empty/zero state with baseline rank. */
    Email() : sender(SenderRank::OTHER_PERSON), subject(""), year(0), month(0), day(0) {}

    /* Parameterized constructor setting all email metadata fields. */
    Email(const SenderRank& sender, const std::string& subject, int year, int month, int day)
        : sender(sender), subject(subject), year(year), month(month), day(day) {}

    // Priority ordering: 
    // 1. Sender category priority
    // 2. Tie-breaker: Newer date takes precedence (discovered at Sprint)
    /* Comparison operator (>):
     * Evaluates relative priority between two emails. First compares sender rank (higher value
     * has higher priority). If ranks match, checks date components sequentially (year, month, day),
     * treating earlier dates as having precedence. */
    bool operator>(const Email& other) const {
        int rankThis = static_cast<int>(sender);
        int rankOther = static_cast<int>(other.sender);

        if (rankThis != rankOther) {
            return rankThis > rankOther;
        }
        if (year != other.year) {
            return year < other.year;
        }
        if (month != other.month) {
            return month < other.month;
        }
        return day < other.day;
    }

    /* Comparison operator (<): Inverts operator> to determine if this email has lower priority. */
    bool operator<(const Email& other) const {
        return other > *this;
    }

    // Written by Gemini, modified by Aaron Hails
    /* Maps a SenderRank enum value to its corresponding human-readable string representation. */
    static const std::string formatSenderName(SenderRank rank) {
        switch (rank) { // Map category enum to printable string
            case SenderRank::BOSS:               return "Boss";
            case SenderRank::SUBORDINATE:        return "Subordinate";
            case SenderRank::PEER:               return "Peer";
            case SenderRank::IMPORTANT_PERSON:   return "ImportantPerson";
            case SenderRank::OTHER_PERSON:       return "OtherPerson";
            default:          return "Unknown";
        }
    }

    // Written by Github Copilot
    /* Converts an input string into its matching SenderRank enum value, defaulting to OTHER_PERSON. */
    static const SenderRank parseSenderName(const std::string& name) {
        if (name == "Boss") {
            return SenderRank::BOSS;
        } else if (name == "Subordinate") {
            return SenderRank::SUBORDINATE;
        } else if (name == "Peer") {
            return SenderRank::PEER;
        } else if (name == "ImportantPerson") {
            return SenderRank::IMPORTANT_PERSON;
        } else if (name == "OtherPerson") {
            return SenderRank::OTHER_PERSON;
        } else {
            return SenderRank::OTHER_PERSON;
        }
    }

    // Written by Gemini
    /* Outputs formatted email details (sender, subject, date) to standard output. */
    void display() const {
        std::cout << "Sender: " << formatSenderName(sender) << "\n";
        std::cout << "Subject: " << subject << "\n";
        std::cout << "Date: " << getDateString() << "\n";
    }
};

// MaxHeap implemented from scratch using a dynamically resizing array
// Written by Gemini
class MaxHeap {
private:
    /* Internal array pointer, total allocated capacity, and current element count. */
    Email* heap;
    int capacity;
    int currentSize;

    /* Doubles array capacity by allocating a new buffer, copying existing elements,
     * deallocating old storage, and updating capacity tracking. */
    void resize() {
        int newCapacity = capacity * 2;
        Email* newHeap = new Email[newCapacity];
        for (int i = 0; i < currentSize; ++i) {
            newHeap[i] = heap[i];
        }
        delete[] heap;
        heap = newHeap;
        capacity = newCapacity;
    }

    /* Restores the max-heap property upwards from a specified leaf index by repeatedly
     * swapping with its parent node until the parent has greater or equal priority. */
    void siftUp(int index) {
        while (index > 0) {
            int parent = (index - 1) / 2;
            if (heap[index] > heap[parent]) {
                swap(heap[index], heap[parent]);
                index = parent;
            } else {
                break;
            }
        }
    }

    /* Restores the max-heap property downwards from the root by finding the highest-priority
     * child among valid left and right branches and swapping downwards until order is restored. */
    void siftDown(int index) {
        while (true) {
            int leftChild = 2 * index + 1;
            int rightChild = 2 * index + 2;
            int largest = index;

            if (leftChild < currentSize && heap[leftChild] > heap[largest]) {
                largest = leftChild;
            }
            if (rightChild < currentSize && heap[rightChild] > heap[largest]) {
                largest = rightChild;
            }

            if (largest != index) {
                swap(heap[index], heap[largest]);
                index = largest;
            } else {
                break;
            }
        }
    }

    // Written by Gemini
    /* Swaps two Email instances using move semantics for performance. */
    void swap(Email& a, Email& b) noexcept {
        Email temp = std::move(a);
        a = std::move(b);
        b = std::move(temp);
    }

public:
    // Written by Gemini
    /* Initializes heap storage with specified initial capacity (default 16). */
    MaxHeap(int initialCapacity = 16) 
        : capacity(initialCapacity), currentSize(0) {
        heap = new Email[capacity];
    }

    /* Destructor: frees the dynamically allocated Email array buffer. */
    ~MaxHeap() {
        delete[] heap;
    }

    /* Inserts a new email: resizes buffer if capacity is reached, places email at the end,
     * sifts up to maintain heap order, and increments element count. */
    void insert(const Email& email) {
        if (currentSize == capacity) {
            resize();
        }
        heap[currentSize] = email;
        siftUp(currentSize);
        currentSize++;
    }

    // Access the highest priority email without removing it
    /* Returns false if empty; otherwise copies the root (max-priority) email to outEmail. */
    bool peek(Email& outEmail) const {
        if (currentSize == 0) return false;
        outEmail = heap[0];
        return true;
    }

    // Delete the highest priority email
    /* Removes root email: replaces root with the last element, decrements size,
     * and sifts down from the root to restore the heap property. */
    bool removeMax() {
        if (currentSize == 0) return false;
        heap[0] = heap[currentSize - 1];
        currentSize--;
        if (currentSize > 0) {
            siftDown(0);
        }
        return true;
    }

    /* Returns the total number of elements currently stored in the heap. */
    int size() const {
        return currentSize;
    }

    /* Returns true if the heap contains zero elements. */
    bool isEmpty() const {
        return currentSize == 0;
    }
};

// Priority Queue abstraction using the MaxHeap
// Written by Gemini
/* Adapter class providing standard FIFO-priority queue operations over the MaxHeap. */
class PriorityQueue {
private:
    MaxHeap heap;

public:
    /* Inserts an email into the underlying priority queue. */
    void enqueue(const Email& email) {
        heap.insert(email);
    }

    /* Removes the highest-priority email from the queue; returns false if empty. */
    bool dequeue() {
        return heap.removeMax();
    }

    /* Inspects the top email without removing it; returns false if empty. */
    bool peek(Email& email) const {
        return heap.peek(email);
    }

    /* Returns the count of items in the queue. */
    int count() const {
        return heap.size();
    }
};

// Command handler for processing CEO inbox instructions
// Written by Gemini
class CEOInboxManager {
private:
    PriorityQueue queue;

    /* Utility function that strips leading and trailing whitespace characters (spaces, tabs, newlines). */
    std::string trim(const std::string& str) {
        size_t first = str.find_first_not_of(" \t\r\n");
        if (first == std::string::npos) return "";
        size_t last = str.find_last_not_of(" \t\r\n");
        return str.substr(first, (last - first + 1));
    }

public:
    /* Dispatches commands based on line content:
     * - "EMAIL <payload>": Parses sender, subject, and date components via comma separation,
     *   validates YYYY-MM-DD date format, and enqueues the Email.
     * - "COUNT": Prints pending email count.
     * - "NEXT": Peeks and prints the next email or displays an empty queue notice.
     * - "READ": Pops the highest-priority email from the queue.
     * - Otherwise: Prints unrecognized command error to std::cerr. */
    void processLine(const std::string& rawLine) {
        std::string line = trim(rawLine);
        if (line.empty()) return;

        if (line.rfind("EMAIL", 0) == 0) {
            // Extract the payload after the "EMAIL " keyword
            std::string payload = line.substr(5);
            std::stringstream ss(payload);
            std::string sender, subject, date;

            if (std::getline(ss, sender, ',') &&
                std::getline(ss, subject, ',') &&
                std::getline(ss, date))
            {
                SenderRank rank = SenderRank::OTHER_PERSON;
                // Written by Github Copilot
                int year, month, day;
                if (std::sscanf(trim(date).c_str(), "%d-%d-%d", &year, &month, &day) == 3) {
                    queue.enqueue(Email(Email::parseSenderName(trim(sender)), trim(subject), year, month, day));
                } else {
                    std::cerr << "Error parsing date: " << date << "\n";
                }
            }
        } 
        else if (line == "COUNT") {
            std::cout << "There are " << queue.count() << " emails to read.\n";
        } 
        else if (line == "NEXT") {
            Email nextEmail;
            if (queue.peek(nextEmail)) {
                std::cout << "Next email:\n";
                nextEmail.display();
            } else {
                std::cout << "No emails to read.\n";
            }
        } 
        else if (line == "READ") {
            if (queue.count() > 0) {
                queue.dequeue();
            } else {
                std::cout << "No emails to read.\n";
            }
        } else {
            std::cerr << "Unknown command: " << line << "\n";
        }
    }

    // Written by Gemini
    /* Reads incoming lines sequentially from the provided input stream until EOF. */
    void run(std::istream& in) {
        std::string line;
        while (std::getline(in, line)) {
            processLine(line);
        }
    }
};

// Written by Gemini
/* Entry point: instantiates CEOInboxManager and processes standard input commands. */
int main() {
    CEOInboxManager manager;
    manager.run(std::cin);
    return 0;
}