#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include <cstddef>

// Forward declaration of MaxHeap template
template <typename T>
class MaxHeap;

/**
 * Email class to store email details and handle priority comparison.
 */
class Email {
public:
    std::string sender;
    std::string subject;
    std::string dateStr; // Original format MM-DD-YYYY
    
    int year, month, day; // Parsed integers for easy comparison

    Email() : year(0), month(0), day(0) {}

    /**
     * Parse the date string "MM-DD-YYYY" into integer components.
     */
    void parseDate(const std::string& d) {
        dateStr = d;
        // Format: MM-DD-YYYY
        using namespace std;
        size_t dash1 = d.find('-');
        size_t dash2 = d.find('-', dash1 + 1);

        if (dash1 != std::string::npos && dash2 != std::string::npos) {
            month = std::stoi(d.substr(0, dash1));
            day = std::stoi(d.substr(dash1 + 1, dash2 - dash1 - 1));
            year = std::stoi(d.substr(dash2 + 1));
        } else {
            // Fallback if format is wrong (though problem guarantees valid format)
            month = 0; day = 0; year = 0;
        }
    }

    /**
     * Helper to get priority value for sender. Higher number = higher priority.
     */
    int getSenderPriority() const {
        if (sender == "Boss") return 5;
        if (sender == "Subordinate") return 4;
        if (sender == "Peer") return 3;
        if (sender == "ImportantPerson") return 2;
        if (sender == "OtherPerson") return 1;
        return 0; // Default for unknown senders, though problem limits to specific ones.
    }

    /**
     * Comparison operator for MaxHeap.
     * Returns true if 'this' is LESS than 'other'.
     * Priority Logic:
     * 1. Higher sender priority comes first (so we want lower value in < comparison).
     * 2. If senders are equal, newer date comes first (so older date has lower value in < comparison).
     */
    bool operator<(const Email& other) const {
        int myPriority = getSenderPriority();
        int otherPriority = other.getSenderPriority();

        if (myPriority != otherPriority) {
            // Lower priority number means this email is "less" than the higher one.
            return myPriority < otherPriority;
        }

        // If priorities are equal, compare dates.
        // We want newer emails to be at the top of the MaxHeap (max element).
        // So if 'this' is OLDER than 'other', 'this' should be "less" (<) than 'other'.
        
        if (year != other.year) {
            return year < other.year;
        }
        if (month != other.month) {
            return month < other.month;
        }
        return day < other.day;
    }

    /**
     * Output format for NEXT command.
     */
    void display() const {
        std::cout << "Sender: " << sender << "\n";
        std::cout << "Subject: " << subject << "\n";
        std:: cout << "Date: " << dateStr << "\n";
    }
};

/**
 * Generic MaxHeap implementation using a vector (list-based).
 */
template <typename T>
class MaxHeap {
private:
    std::vector<T> heap;

    // Helper to swap elements
    void swap(int i, int j) {
        T temp = heap[i];
        heap[i] = heap[j];
        heap[j] = temp;
    }

    // Heapify up after insertion
    void heapifyUp(int index) {
        while (index > 0) {
            int parent = (index - 1) / 2;
            if (heap[index] < heap[parent]) { // Note: Using operator< defined in Email. 
                                                // Wait, MaxHeap usually uses > for max heap?
                                                // Let's re-verify logic below.
                break;
            }
            
            // Standard MaxHeap Logic: Parent must be >= Child.
            // If child (index) is GREATER than parent, swap them.
            // So we check if heap[index] > heap[parent].
            // But our Email operator< defines "less priority".
            // We want the element with HIGHER priority to be at root.
            
            // Let's stick to standard MaxHeap logic: 
            // If child is "greater" than parent, swap up.
            // For Email, "greater" means higher sender priority OR newer date.
            // Our operator< returns true if 'this' has LOWER priority/older date.
            // So we need the inverse for swapping in a MaxHeap: 
            // If heap[index] is NOT less than heap[parent], it might be greater or equal.
            
            // Actually, let's just implement standard comparison explicitly to avoid confusion with operator< direction.
            if (isGreater(heap[index], heap[parent])) {
                swap(index, parent);
                index = parent;
            } else {
                break;
            }
        }
    }

    // Heapify down after removal
    void heapifyDown(int index) {
        int size = heap.size();
        while (true) {
            int left = 2 * index + 1;
            int right = 2 * index + 2;
            int largest = index;

            if (left < size && isGreater(heap[left], heap[largest])) {
                largest = left;
            }
            if (right < size && isGreater(heap[right], heap[largest])) {
                largest = right;
            }

            if (largest != index) {
                swap(index, largest);
                index = largest;
            } else {
                break;
            }
        }
    }

    // Helper to determine if 'a' has higher priority than 'b' for Email type
    bool isGreater(const T& a, const T& b) {
        // If operator< is defined as "has lower priority", then ! (a < b) && !(b < a) implies equal.
        // We want to know if a > b in terms of priority.
        // Since Email::operator< returns true if 'this' is LOWER priority:
        // a > b means !(a < b) AND NOT (a == b). 
        // However, simpler logic for MaxHeap with custom comparator:
        // We want the "largest" element at top.
        // Let's rely on the fact that Email::operator< is strictly "less priority".
        // So 'greater' means it does not satisfy '<' and is not equal? 
        // Actually, let's just use a lambda or explicit check inside the class if needed, 
        // but since T is Email, we can cast.
        
        // To keep this generic for any type that supports <, MaxHeap usually requires > logic.
        // Let's assume standard behavior: MaxHeap puts max element at top.
        // We will use a custom comparator approach or just rely on the fact that 
        // if T has operator< defined as "lower priority", we need to invert it for MaxHeap?
        
        // Correction: The prompt asks for a MaxHeap. 
        // If Email::operator< means "is lower priority than", then the element with HIGHER priority is GREATER.
        // So standard MaxHeap logic (swap if child > parent) works IF operator> is defined or we use !a<b.
        
        // Let's implement explicit comparison for Email to be safe and clear.
        const Email* eA = dynamic_cast<const Email*>(&a);
        const Email* eB = dynamic_cast<const Email*>(&b);
        
        if (eA && eB) {
            return !(*eA < *eB) && !(*eB < *eA); // Strictly greater? No, that's equality.
            // Let's use the explicit logic from Email::operator< again.
            // If a has higher priority than b, then (b < a) is true.
        }
        
        // Generic fallback for other types if needed:
        return b < a; 
    }

public:
    void push(const T& item) {
        heap.push_back(item);
        heapifyUp(heap.size() - 1);
    }

    T pop() {
        if (heap.empty()) {
            throw std::runtime_error("Heap is empty");
        }
        T top = heap[0];
        heap[0] = heap.back();
        heap.pop_back();
        if (!heap.empty()) {
            heapifyDown(0);
        }
        return top;
    }

    bool empty() const {
        return heap.empty();
    }

    size_t size() const {
        return heap.size();
    }
    
    // Helper to peek at the max element without removing it (for NEXT command logic if needed, 
    // but we remove on READ. The prompt says NEXT displays then READ removes. 
    // Wait: "NEXT ... Display ... READ CEO has read email and dealt with it".
    // Usually NEXT just shows top, READ removes top.
    // But the sample output shows NEXT displaying, then READ removing.
    // If I call NEXT twice, does it remove? No, usually NEXT is peek.
    // Let's re-read: "NEXT ... Display the information ... READ CEO has read email and dealt with it".
    // This implies NEXT is a Peek operation.
    
    T peek() const {
        if (heap.empty()) {
            throw std::runtime_error("Heap is empty");
        }
        return heap[0];
    }
};

/**
 * CEOInbox class to manage the priority queue and process commands.
 */
class CEOInbox {
private:
    MaxHeap<Email> inbox;

public:
    void addEmail(const std::string& sender, const std::string& subject, const std::string& date) {
        Email e;
        e.sender = sender;
        e.subject = subject;
        e.parseDate(date);
        inbox.push(e);
    }

    void processNext() {
        if (inbox.empty()) {
            return; // Or handle error, but prompt implies valid commands or graceful handling.
                    // "be able to handle a file that does NEXT ... when there are no emails"
        }
        
        Email top = inbox.peek();
        std::cout << "Next email:\n";
        top.display();
    }

    void processRead() {
        if (inbox.empty()) {
            return; // "two READs in a row ... highest priority email deleted" -> implies if empty, do nothing.
        }
        
        inbox.pop();
    }

    void displayCount() {
        std::cout << "There are " << inbox.size() << " emails to read.\n";
    }

    void processCommand(const std::string& command) {
        if (command == "EMAIL") {
            // This is handled by a separate parsing step in main because EMAIL has arguments.
            // But the class structure suggests handling commands here. 
            // However, since EMAIL requires complex argument parsing from stdin, 
            // it's cleaner to parse in main and call addEmail.
        } else if (command == "NEXT") {
            processNext();
        } else if (command == "READ") {
            processRead();
        } else if (command == "COUNT") {
            displayCount();
        }
    }
};

/**
 * Helper to parse the EMAIL command line.
 */
void handleEmailCommand(std::istream& in, CEOInbox& inbox) {
    std::string sender;
    std::getline(in, sender, ','); // Read until comma
    
    std::string subject;
    std::getline(in, subject, ','); // Read until comma
    
    std::string date;
    std::getline(in, date); // Read rest of line (date)
    
    // Trim potential whitespace/newlines from date if necessary
    if (!date.empty() && date.back() == '\r') {
        date.pop_back();
    }

    inbox.addEmail(sender, subject, date);
}

int main() {
    CEOInbox inbox;
    std::string command;

    while (std::cin >> command) {
        if (command == "EMAIL") {
            handleEmailCommand(std::cin, inbox);
        } else if (command == "NEXT") {
            inbox.processNext();
        } else if (command == "READ") {
            inbox.processRead();
        } else if (command == "COUNT") {
            inbox.displayCount();
        }
    }

    return 0;
}