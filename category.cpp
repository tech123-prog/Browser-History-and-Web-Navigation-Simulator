#include "navigation.h"

// Calculate the number of websites in each category
unordered_map<string, int>
BrowserSimulator::getCategoryDistribution() {

    unordered_map<string, int> distribution;

    // Count each website according to its category
    for (const auto& website : websiteCategory) {

        string category = website.second;

        distribution[category]++;
    }

    return distribution;
}