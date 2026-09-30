#include "common.h"

Page* head = NULL;
Page* tail = NULL;
Page* currentPage = NULL;

stack<Page*> backStack;
stack<Page*> forwardStack;

queue<string> recentWebsites;

unordered_map<string, string> websiteCategory;
unordered_map<string, int> visitFrequency;
unordered_map<string, int> timeLimit;

int main()
{
    cout << "Web Navigation System" << endl;

    return 0;
}