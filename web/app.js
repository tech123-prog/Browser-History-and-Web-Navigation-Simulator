
const API = "http://localhost:8080";

async function request(path) {
    try {
        const response = await fetch(API + path);
        const data = await response.json();

        if (data.error) {
            showMessage(data.error);
            return null;
        }

        return data;
    }
    catch (error) {
        showMessage("C++ server is not running. Start main.cpp first.");
        return null;
    }
}

function showMessage(message) {
    document.getElementById("message").textContent = message;
}

function updatePage(data) {
    if (!data) return;

    document.getElementById("current").textContent =
        data.currentWebsite || "No website visited yet.";

    const recent = document.getElementById("recent");
    recent.innerHTML = "";

    data.recentWebsites.forEach(url => {
        const li = document.createElement("li");
        li.textContent = url;
        recent.appendChild(li);
    });

    const visits = document.getElementById("visits");
    visits.innerHTML = "";

    Object.entries(data.visitFrequency).forEach(([url, count]) => {
        const li = document.createElement("li");
        li.textContent = url + " : " + count + " visits";
        visits.appendChild(li);
    });

    const categories = document.getElementById("categories");
    categories.innerHTML = "";

    Object.entries(data.categoryDistribution).forEach(([category, count]) => {
        const li = document.createElement("li");
        li.textContent = category + " : " + count + " websites";
        categories.appendChild(li);
    });
}

async function refresh() {
    const data = await request("/state");
    updatePage(data);
}

async function visit() {
    const url = document.getElementById("url").value.trim();
    const category = document.getElementById("category").value;

    if (!url) {
        showMessage("Enter a website URL first.");
        return;
    }

    const path = "/visit?url=" + encodeURIComponent(url) +
                 "&category=" + encodeURIComponent(category);

    const data = await request(path);
    updatePage(data);

    if (data) {
        document.getElementById("url").value = "";
        showMessage("Website visited using C++.");
    }
}

async function goBack() {
    const data = await request("/back");
    updatePage(data);
    if (data) showMessage("Back operation performed by C++.");
}

async function goForward() {
    const data = await request("/forward");
    updatePage(data);
    if (data) showMessage("Forward operation performed by C++.");
}

async function searchWebsite() {
    const keyword = document.getElementById("keyword").value.trim();

    if (!keyword) {
        showMessage("Enter a search keyword.");
        return;
    }

    const results = await request("/search?keyword=" +
                                  encodeURIComponent(keyword));

    if (!results) return;

    const list = document.getElementById("search-results");
    list.innerHTML = "";

    if (results.length === 0) {
        const li = document.createElement("li");
        li.textContent = "No matching websites found.";
        list.appendChild(li);
        return;
    }

    results.forEach(item => {
        const li = document.createElement("li");
        li.textContent = item.url + " (" + item.visits + " visits)";
        list.appendChild(li);
    });
}

async function setLimit() {
    const url = document.getElementById("limit-url").value.trim();
    const minutes = document.getElementById("minutes").value;

    if (!url || !minutes || Number(minutes) < 1) {
        showMessage("Enter a URL and a valid time limit.");
        return;
    }

    const result = await request(
        "/limit?url=" + encodeURIComponent(url) +
        "&minutes=" + encodeURIComponent(minutes)
    );

    if (result) {
        document.getElementById("limit-result").textContent =
            "Time limit saved in C++.";
        showMessage("Time limit saved.");
    }
}

async function checkLimit() {
    let limitTriggered = false;

async function checkLimit() {
    if (limitTriggered) return;

    const url = document.getElementById("current").textContent;

    if (!url || url === "No website visited yet.") return;

    const result = await request(
        "/check-limit?url=" + encodeURIComponent(url)
    );

    if (result && result.exceeded) {
        limitTriggered = true;

        showMessage("Time limit exceeded! Returning to previous page.");

        const data = await request("/back");
        updatePage(data);

        setTimeout(() => {
            limitTriggered = false;
        }, 1000);
    }
}
}

refresh();
setInterval(checkLimit, 5000);
