#ifndef COMMON_H
#define COMMON_H

#include <iostream>
#include <string>
#include <stack>
#include <queue>
#include <unordered_map>
#include <ctime>
#include <sstream>
#include <algorithm>
#include <vector>

using namespace std;

// Node for the browsing history
struct Page {
    string url;
    string category;

    time_t startTime;
    int duration;

    Page* prev;
    Page* next;

    Page(string website, string type) {
        url = website;
        category = type;
        startTime = time(nullptr);
        duration = 0;

        prev = nullptr;
        next = nullptr;
    }
};

#endif