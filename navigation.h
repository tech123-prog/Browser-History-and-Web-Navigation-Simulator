#ifndef NAVIGATION_H
#define NAVIGATION_H

#include "common.h"

class BrowserSimulator {

private:

    // Doubly linked list
    Page* head;
    Page* tail;
    Page* current;

    // Back and Forward stacks
    stack<Page*> backStack;
    stack<Page*> forwardStack;

    // Recent websites queue
    queue<string> recent;

    // Hash maps
    unordered_map<string, string> websiteCategory;
    unordered_map<string, int> visitFrequency;
    unordered_map<string, int> timeLimit;

    // Helper functions
    void stopTimer(Page* page);
    int currentElapsed() const;
    string escapeJSON(const string& value) const;

public:

    // Constructor and destructor
    BrowserSimulator();
    ~BrowserSimulator();

    // Navigation functions
    void visit(string url, string category);
    void goBack();
    void goForward();

    // Queue functions
    void addRecent(string url);
    vector<string> getRecent();

    // Time limit functions
    void setWebsiteLimit(string url, int minutes);
    bool checkTimeLimit(string url);

    // Category functions
    unordered_map<string, int> getCategoryDistribution();

    // Search and state
    string search(string keyword);
    string getState();
};

#endif