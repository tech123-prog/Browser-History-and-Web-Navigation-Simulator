#include "navigation.h"

// Set a time limit for a website (in minutes)
void BrowserSimulator::setWebsiteLimit(string url, int minutes) {

    if (url.empty() || minutes <= 0) {
        cout << "Please enter a valid website and time limit."
             << endl;
        return;
    }

    timeLimit[url] = minutes;

    cout << "Time limit set for " << url
         << ": " << minutes << " minutes." << endl;
}

// Check whether the website has exceeded its time limit
bool BrowserSimulator::checkTimeLimit(string url) {

    if (current == nullptr || current->url != url) {
        return false;
    }

    if (timeLimit.find(url) == timeLimit.end()) {
        return false;
    }

    int elapsedSeconds = currentElapsed();
    int limitSeconds = timeLimit[url] * 60;

    if (elapsedSeconds >= limitSeconds) {
        cout << "WARNING: Time limit exceeded for "
             << url << "!" << endl;

        return true;
    }

    return false;
}