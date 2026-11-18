#include "navigation.h"

// Constructor
BrowserSimulator::BrowserSimulator() {
    head = nullptr;
    tail = nullptr;
    current = nullptr;
}

// Destructor
BrowserSimulator::~BrowserSimulator() {
    Page* temp = head;

    while (temp != nullptr) {
        Page* nextPage = temp->next;
        delete temp;
        temp = nextPage;
    }
}

// Stop the timer for a page
void BrowserSimulator::stopTimer(Page* page) {
    if (page != nullptr) {
        int elapsed = static_cast<int>(
            difftime(time(nullptr), page->startTime)
        );

        page->duration += elapsed;
    }
}

// Get elapsed time for the current page
int BrowserSimulator::currentElapsed() const {
    if (current == nullptr) {
        return 0;
    }

    return current->duration +
        static_cast<int>(
            difftime(time(nullptr), current->startTime)
        );
}

// Visit a website
void BrowserSimulator::visit(string url, string category) {

    if (url.empty()) {
        cout << "Please enter a website URL." << endl;
        return;
    }

    // Stop timer of previous website
    if (current != nullptr) {
        stopTimer(current);
        backStack.push(current);
    }

    // Clear forward history when a new page is visited
    while (!forwardStack.empty()) {
        forwardStack.pop();
    }

    // Create a new page
    Page* newPage = new Page(url, category);

    // Add page to doubly linked list
    if (head == nullptr) {
        head = tail = newPage;
    } else {
        tail->next = newPage;
        newPage->prev = tail;
        tail = newPage;
    }

    current = newPage;

    // Update website information
    websiteCategory[url] = category;
    visitFrequency[url]++;

    // Add website to recent queue
    addRecent(url);

    cout << "Website visited: " << url << endl;
}

// Go Back
void BrowserSimulator::goBack() {

    if (backStack.empty()) {
        cout << "No previous website available." << endl;
        return;
    }

    if (current != nullptr) {
        stopTimer(current);
        forwardStack.push(current);
    }

    current = backStack.top();
    backStack.pop();

    current->startTime = time(nullptr);

    cout << "Moved back to: " << current->url << endl;
}

// Go Forward
void BrowserSimulator::goForward() {

    if (forwardStack.empty()) {
        cout << "No forward website available." << endl;
        return;
    }

    if (current != nullptr) {
        stopTimer(current);
        backStack.push(current);
    }

    current = forwardStack.top();
    forwardStack.pop();

    current->startTime = time(nullptr);

    cout << "Moved forward to: " << current->url << endl;
}