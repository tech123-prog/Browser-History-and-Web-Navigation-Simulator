#ifndef COMMON_H
#define COMMON_H

#include <iostream>
#include <string>
#include <stack>
#include <queue>
#include <unordered_map>
#include <ctime>

using namespace std;

struct Page
{
    string url;
    string category;
    time_t startTime;
    int duration;

    Page* prev;
    Page* next;
};

extern Page* head;
extern Page* tail;
extern Page* currentPage;

extern stack<Page*> backStack;
extern stack<Page*> forwardStack;

extern queue<string> recentWebsites;

extern unordered_map<string, string> websiteCategory;
extern unordered_map<string, int> visitFrequency;
extern unordered_map<string, int> timeLimit;

#endif