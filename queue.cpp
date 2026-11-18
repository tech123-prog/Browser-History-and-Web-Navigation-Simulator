#include "navigation.h"

// Add a website to the recent websites queue
void BrowserSimulator::addRecent(string url) {

    // Remove the oldest website if queue is full
    if (recent.size() == 5) {
        recent.pop();
    }

    // Add the latest website
    recent.push(url);
}

// Return the five most recent websites
vector<string> BrowserSimulator::getRecent() {

    vector<string> websites;

    queue<string> temp = recent;

    while (!temp.empty()) {
        websites.push_back(temp.front());
        temp.pop();
    }

    return websites;
}