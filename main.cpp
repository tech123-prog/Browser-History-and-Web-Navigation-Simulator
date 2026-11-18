
#include "navigation.h"
#include <cctype>
#include <cstring>
#include <cstdlib>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
typedef SOCKET Socket;
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
typedef int Socket;
#endif

using namespace std;

string BrowserSimulator::escapeJSON(const string& value) const {
    string result;

    for (char ch : value) {
        if (ch == '"') result += "\\\"";
        else if (ch == '\\') result += "\\\\";
        else if (ch == '\n') result += "\\n";
        else if (ch == '\r') result += "\\r";
        else result += ch;
    }

    return result;
}

string BrowserSimulator::search(string keyword) {
    string result = "[";
    bool first = true;

    for (auto item : visitFrequency) {
        string url = item.first;
        string a = url;
        string b = keyword;

        transform(a.begin(), a.end(), a.begin(), ::tolower);
        transform(b.begin(), b.end(), b.begin(), ::tolower);

        if (a.find(b) != string::npos) {
            if (!first) result += ",";

            result += "{\"url\":\"" + escapeJSON(url) +
                      "\",\"category\":\"" +
                      escapeJSON(websiteCategory[url]) +
                      "\",\"visits\":" + to_string(item.second) + "}";

            first = false;
        }
    }

    result += "]";
    return result;
}

string BrowserSimulator::getState() {
    string result = "{";

    result += "\"currentWebsite\":\"";

    if (current != nullptr)
        result += escapeJSON(current->url);

    result += "\",\"recentWebsites\":[";

    vector<string> recentList = getRecent();

    for (int i = 0; i < recentList.size(); i++) {
        if (i > 0) result += ",";
        result += "\"" + escapeJSON(recentList[i]) + "\"";
    }

    result += "],\"visitFrequency\":{";

    bool first = true;

    for (auto item : visitFrequency) {
        if (!first) result += ",";

        result += "\"" + escapeJSON(item.first) + "\":" +
                  to_string(item.second);

        first = false;
    }

    result += "},\"categoryDistribution\":{";

    unordered_map<string, int> distribution = getCategoryDistribution();
    first = true;

    for (auto item : distribution) {
        if (!first) result += ",";

        result += "\"" + escapeJSON(item.first) + "\":" +
                  to_string(item.second);

        first = false;
    }

    result += "}}";

    return result;
}

string decodeURL(string value) {
    string result;

    for (int i = 0; i < value.length(); i++) {
        if (value[i] == '+') {
            result += ' ';
        }
        else if (value[i] == '%' && i + 2 < value.length()) {
            string hex = value.substr(i + 1, 2);
            result += (char)strtol(hex.c_str(), nullptr, 16);
            i += 2;
        }
        else {
            result += value[i];
        }
    }

    return result;
}

string getParameter(string query, string key) {
    string searchKey = key + "=";
    size_t position = query.find(searchKey);

    if (position == string::npos) return "";

    position += searchKey.length();

    size_t end = query.find("&", position);

    string value;

    if (end == string::npos)
        value = query.substr(position);
    else
        value = query.substr(position, end - position);

    return decodeURL(value);
}

string handleRequest(BrowserSimulator& browser, string request) {
    size_t start = request.find(" ");
    size_t end = request.find(" ", start + 1);

    if (start == string::npos || end == string::npos)
        return "{\"error\":\"Invalid request\"}";

    string path = request.substr(start + 1, end - start - 1);

    size_t question = path.find("?");
    string route = path.substr(0, question);
    string query = "";

    if (question != string::npos)
        query = path.substr(question + 1);

    if (route == "/state") {
        return browser.getState();
    }

    if (route == "/visit") {
        string url = getParameter(query, "url");
        string category = getParameter(query, "category");

        if (url.empty() || category.empty())
            return "{\"error\":\"URL and category are required\"}";

        browser.visit(url, category);
        browser.addRecent(url);

        return browser.getState();
    }

    if (route == "/back") {
        browser.goBack();
        return browser.getState();
    }

    if (route == "/forward") {
        browser.goForward();
        return browser.getState();
    }

    if (route == "/search") {
        string keyword = getParameter(query, "keyword");
        return browser.search(keyword);
    }

    if (route == "/limit") {
        string url = getParameter(query, "url");
        string minutes = getParameter(query, "minutes");

        if (url.empty() || minutes.empty())
            return "{\"error\":\"URL and minutes are required\"}";

        browser.setWebsiteLimit(url, stoi(minutes));
        return "{\"message\":\"Time limit saved\"}";
    }

    if (route == "/check-limit") {
        bool exceeded = browser.checkTimeLimit(
            getParameter(query, "url")
        );

        return exceeded ? "{\"exceeded\":true}" :
                          "{\"exceeded\":false}";
    }

    return "{\"error\":\"Unknown route\"}";
}

void closeSocket(Socket client) {
#ifdef _WIN32
    closesocket(client);
#else
    close(client);
#endif
}

int main() {
#ifdef _WIN32
    WSADATA data;

    if (WSAStartup(MAKEWORD(2, 2), &data) != 0) {
        cout << "Socket startup failed.\n";
        return 1;
    }
#endif

    Socket server = socket(AF_INET, SOCK_STREAM, 0);

    if (server < 0) {
        cout << "Could not create server.\n";
        return 1;
    }

    int option = 1;

#ifdef _WIN32
    setsockopt(server, SOL_SOCKET, SO_REUSEADDR,
               (const char*)&option, sizeof(option));
#else
    setsockopt(server, SOL_SOCKET, SO_REUSEADDR,
               &option, sizeof(option));
#endif

    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_port = htons(8080);
    address.sin_addr.s_addr = INADDR_ANY;

    if (bind(server, (sockaddr*)&address, sizeof(address)) < 0) {
        cout << "Could not bind to port 8080.\n";
        closeSocket(server);
        return 1;
    }

    if (listen(server, 10) < 0) {
        cout << "Server could not start listening.\n";
        closeSocket(server);
        return 1;
    }

    BrowserSimulator browser;

    cout << "\nBrowser Navigation Simulator is running!\n";
    cout << "Open http://localhost:8080 in your browser.\n";
    cout << "Keep this terminal open while using the website.\n\n";

    while (true) {
        Socket client = accept(server, nullptr, nullptr);

        if (client < 0) continue;

        char buffer[8192] = {0};
        int bytes = recv(client, buffer, sizeof(buffer) - 1, 0);

        if (bytes > 0) {
            string request(buffer, bytes);
            string body;

            if (request.find("OPTIONS ") == 0) {
                body = "";
            }
            else {
                body = handleRequest(browser, request);
            }

            string response =
                "HTTP/1.1 200 OK\r\n"
                "Access-Control-Allow-Origin: *\r\n"
                "Access-Control-Allow-Methods: GET, OPTIONS\r\n"
                "Access-Control-Allow-Headers: Content-Type\r\n"
                "Content-Type: application/json\r\n"
                "Connection: close\r\n"
                "Content-Length: " + to_string(body.length()) +
                "\r\n\r\n" + body;

            send(client, response.c_str(), (int)response.length(), 0);
        }

        closeSocket(client);
    }

    closeSocket(server);

#ifdef _WIN32
    WSACleanup();
#endif

    return 0;
}
