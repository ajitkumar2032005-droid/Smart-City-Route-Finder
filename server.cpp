
#include <algorithm>
#include <cmath>
#include <cstring>
#include <functional>
#include <fstream>
#include <iostream>
#include <queue>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <utility>

#ifdef _WIN32
    #ifndef NOMINMAX
        #define NOMINMAX
    #endif

    #include <winsock2.h>
    #include <ws2tcpip.h>

    using socket_t = SOCKET;
    #define CLOSESOCKET closesocket
#else
    #include <arpa/inet.h>
    #include <netinet/in.h>
    #include <sys/socket.h>
    #include <unistd.h>

    using socket_t = int;
    #define INVALID_SOCKET (-1)
    #define SOCKET_ERROR (-1)
    #define CLOSESOCKET close
#endif

using namespace std;

struct Edge {
    int to;
    int distance;
};

class CityGraph {
public:

    vector<string> names = {
        "CGC University",
        "Mohali Bus Stand",
        "Phase 7 Market",
        "Fortis Hospital",
        "Mohali Railway Station",
        "Airport Road",
        "Sector 17 Chandigarh",
        "Sukhna Lake"
    };

    vector<vector<Edge> > adj;

    CityGraph() : adj(names.size()) {

        addRoad(0, 1, 8);
        addRoad(0, 2, 5);
        addRoad(0, 3, 11);

        addRoad(1, 2, 4);
        addRoad(1, 4, 6);
        addRoad(1, 5, 9);

        addRoad(2, 3, 3);
        addRoad(2, 4, 7);
        addRoad(3, 5, 6);

        addRoad(4, 5, 4);
        addRoad(4, 6, 12);
        addRoad(5, 6, 10);

        addRoad(6, 7, 7);
        addRoad(5, 7, 14);
    }

    void addRoad(int a, int b, int w) {
        adj[a].push_back(Edge{b, w});
        adj[b].push_back(Edge{a, w});
    }

    int indexOf(const string& name) const {

        for (int i = 0; i < (int)names.size(); i++) {
            if (names[i] == name) {
                return i;
            }
        }

        return -1;
    }

    // DIJKSTRA ALGORITHM

    vector<int> dijkstra(int src, int dest, int& total) const {

        const int INF = 1000000000;

        int n = (int)names.size();

        vector<int> dist(n, INF);
        vector<int> parent(n, -1);

        typedef pair<int, int> State;

        priority_queue<
            State,
            vector<State>,
            greater<State>
        > pq;

        dist[src] = 0;
        pq.push(make_pair(0, src));

        while (!pq.empty()) {

            State current = pq.top();
            pq.pop();

            int d = current.first;
            int u = current.second;

            if (d != dist[u]) {
                continue;
            }

            if (u == dest) {
                break;
            }

            for (const Edge& e : adj[u]) {

                if (dist[u] + e.distance < dist[e.to]) {

                    dist[e.to] = dist[u] + e.distance;
                    parent[e.to] = u;

                    pq.push(
                        make_pair(dist[e.to], e.to)
                    );
                }
            }
        }

        total = dist[dest];

        return buildPath(parent, src, dest);
    }

    // BREADTH FIRST SEARCH

    vector<int> bfs(int src, int dest, int& total) const {

        int n = (int)names.size();

        vector<int> parent(n, -1);
        vector<int> visited(n, 0);

        queue<int> q;

        visited[src] = 1;
        q.push(src);

        while (!q.empty()) {

            int u = q.front();
            q.pop();

            if (u == dest) {
                break;
            }

            for (const Edge& e : adj[u]) {

                if (!visited[e.to]) {

                    visited[e.to] = 1;
                    parent[e.to] = u;

                    q.push(e.to);
                }
            }
        }

        vector<int> path = buildPath(parent, src, dest);

        total = pathDistance(path);

        return path;
    }

    // DEPTH FIRST SEARCH

    vector<int> dfs(int src, int dest, int& total) const {

        int n = (int)names.size();

        vector<int> parent(n, -1);
        vector<int> visited(n, 0);

        function<bool(int)> visit = [&](int u) {

            visited[u] = 1;

            if (u == dest) {
                return true;
            }

            for (const Edge& e : adj[u]) {

                if (!visited[e.to]) {

                    parent[e.to] = u;

                    if (visit(e.to)) {
                        return true;
                    }
                }
            }

            return false;
        };

        visit(src);

        vector<int> path = buildPath(parent, src, dest);

        total = pathDistance(path);

        return path;
    }

    // CALCULATE PATH DISTANCE

    int pathDistance(const vector<int>& path) const {

        int sum = 0;

        for (int i = 1; i < (int)path.size(); i++) {

            int u = path[i - 1];
            int v = path[i];

            for (const Edge& e : adj[u]) {

                if (e.to == v) {
                    sum += e.distance;
                    break;
                }
            }
        }

        return sum;
    }

private:

    // BUILD FINAL ROUTE

    vector<int> buildPath(
        const vector<int>& parent,
        int src,
        int dest
    ) const {

        if (src != dest && parent[dest] == -1) {
            return vector<int>();
        }

        vector<int> path;

        for (int cur = dest; cur != -1; cur = parent[cur]) {

            path.push_back(cur);

            if (cur == src) {
                break;
            }
        }

        if (path.back() != src) {
            return vector<int>();
        }

        reverse(path.begin(), path.end());

        return path;
    }
};

// JSON ESCAPE

static string jsonEscape(const string& s) {

    string out;

    for (char c : s) {

        if (c == '"' || c == '\\') {
            out += '\\';
        }

        if (c == '\n') {
            out += "\\n";
            continue;
        }

        out += c;
    }

    return out;
}

// URL DECODE

static string urlDecode(const string& s) {

    string out;

    for (size_t i = 0; i < s.size(); i++) {

        if (s[i] == '+') {
            out += ' ';
        }
        else if (s[i] == '%' && i + 2 < s.size()) {

            try {
                out += (char)stoi(s.substr(i + 1, 2), NULL, 16);
                i += 2;
            }
            catch (...) {
                out += s[i];
            }
        }
        else {
            out += s[i];
        }
    }

    return out;
}

// PARSE URL PARAMETERS

static unordered_map<string, string> parseQuery(
    const string& target
) {

    unordered_map<string, string> result;

    size_t q = target.find('?');

    if (q == string::npos) {
        return result;
    }

    stringstream ss(target.substr(q + 1));

    string part;

    while (getline(ss, part, '&')) {

        size_t eq = part.find('=');

        if (eq != string::npos) {

            result[
                urlDecode(part.substr(0, eq))
            ] = urlDecode(part.substr(eq + 1));
        }
    }

    return result;
}

// LOCATIONS JSON

static string makeLocationsJson(const CityGraph& g) {

    ostringstream out;

    out << "{\"locations\":[";

    for (size_t i = 0; i < g.names.size(); i++) {

        if (i) {
            out << ",";
        }

        out << "\"" << jsonEscape(g.names[i]) << "\"";
    }

    out << "]}";

    return out.str();
}

// ROUTE JSON

static string makeRouteJson(
    const CityGraph& g,
    const vector<int>& path,
    int distance,
    const string& algorithm
) {

    ostringstream out;

    out << "{\"found\":"
        << (path.empty() ? "false" : "true")

        << ",\"algorithm\":\""
        << jsonEscape(algorithm) << "\""

        << ",\"distanceKm\":"
        << (path.empty() ? 0 : distance)

        << ",\"estimatedMinutes\":"
        << (path.empty() ? 0 : (int)ceil(distance * 3.0))

        << ",\"path\":[";

    for (size_t i = 0; i < path.size(); i++) {

        if (i) {
            out << ",";
        }

        out << "\""
            << jsonEscape(g.names[path[i]])
            << "\"";
    }

    out << "]}";

    return out.str();
}

// READ FRONTEND FILES

static string readFile(const string& path) {

    ifstream file(path.c_str(), ios::binary);

    if (!file) {
        return "";
    }

    ostringstream ss;

    ss << file.rdbuf();

    return ss.str();
}

// SEND HTTP RESPONSE

static void sendResponse(
    socket_t client,
    const string& status,
    const string& type,
    const string& body
) {

    ostringstream response;

    response
        << "HTTP/1.1 " << status << "\r\n"
        << "Content-Type: " << type << "; charset=utf-8\r\n"
        << "Content-Length: " << body.size() << "\r\n"
        << "Access-Control-Allow-Origin: *\r\n"
        << "Connection: close\r\n\r\n"
        << body;

    string data = response.str();

    send(
        client,
        data.c_str(),
        (int)data.size(),
        0
    );
}

// MAIN SERVER

int main(int argc, char** argv) {

    int port = (argc > 1) ? atoi(argv[1]) : 8080;

#ifdef _WIN32

    WSADATA wsa;

    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {

        cerr << "Winsock initialization failed!" << endl;

        return 1;
    }

#endif

    socket_t server = socket(
        AF_INET,
        SOCK_STREAM,
        0
    );

    if (server == INVALID_SOCKET) {

        cerr << "Socket creation failed!" << endl;

        return 1;
    }

    int opt = 1;

    setsockopt(
        server,
        SOL_SOCKET,
        SO_REUSEADDR,
        (const char*)&opt,
        sizeof(opt)
    );

    sockaddr_in address = {};

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons((unsigned short)port);

    if (
        bind(server, (sockaddr*)&address, sizeof(address))
        == SOCKET_ERROR
        ||
        listen(server, 10) == SOCKET_ERROR
    ) {

        cerr << "Could not start server on port "
             << port << endl;

        CLOSESOCKET(server);

#ifdef _WIN32
        WSACleanup();
#endif

        return 1;
    }

    CityGraph graph;

    cout << endl;
    cout << "====================================" << endl;
    cout << "     SMART CITY ROUTE FINDER" << endl;
    cout << "====================================" << endl;
    cout << "Server running at:" << endl;
    cout << "http://localhost:" << port << endl;
    cout << "Press Ctrl+C to stop the server." << endl;
    cout << "====================================" << endl;

    while (true) {

        sockaddr_in clientAddr = {};

#ifdef _WIN32
        int len = sizeof(clientAddr);
#else
        socklen_t len = sizeof(clientAddr);
#endif

        socket_t client = accept(
            server,
            (sockaddr*)&clientAddr,
            &len
        );

        if (client == INVALID_SOCKET) {
            continue;
        }

        char buffer[8192] = {};

        int received = recv(
            client,
            buffer,
            sizeof(buffer) - 1,
            0
        );

        string request(
            buffer,
            received > 0 ? received : 0
        );

        istringstream req(request);

        string method, target, version;

        req >> method >> target >> version;

        if (method != "GET") {

            sendResponse(
                client,
                "405 Method Not Allowed",
                "text/plain",
                "Only GET is supported."
            );
        }

        // LOCATIONS API

        else if (target.find("/api/locations") == 0) {

            sendResponse(
                client,
                "200 OK",
                "application/json",
                makeLocationsJson(graph)
            );
        }

        // ROUTE API

        else if (target.find("/api/route") == 0) {

            unordered_map<string, string> params =
                parseQuery(target);

            string from = params["from"];
            string to = params["to"];
            string algorithm = params["algorithm"];

            int src = graph.indexOf(from);
            int dest = graph.indexOf(to);

            int distance = 0;

            if (
                src < 0 ||
                dest < 0 ||
                src == dest
            ) {

                sendResponse(
                    client,
                    "400 Bad Request",
                    "application/json",
                    "{\"error\":\"Choose two different valid locations.\"}"
                );
            }
            else {

                vector<int> path;

                if (algorithm == "bfs") {

                    path = graph.bfs(
                        src,
                        dest,
                        distance
                    );
                }
                else if (algorithm == "dfs") {

                    path = graph.dfs(
                        src,
                        dest,
                        distance
                    );
                }
                else {

                    algorithm = "dijkstra";

                    path = graph.dijkstra(
                        src,
                        dest,
                        distance
                    );
                }

                sendResponse(
                    client,
                    "200 OK",
                    "application/json",
                    makeRouteJson(
                        graph,
                        path,
                        distance,
                        algorithm
                    )
                );
            }
        }

        // FRONTEND FILES

        else {

            string path = "public/index.html";
            string type = "text/html";

            if (target == "/styles.css") {

                path = "public/styles.css";
                type = "text/css";
            }
            else if (target == "/app.js") {

                path = "public/app.js";
                type = "application/javascript";
            }
            else if (
                target != "/" &&
                target != "/index.html"
            ) {

                sendResponse(
                    client,
                    "404 Not Found",
                    "text/plain",
                    "Not found"
                );

                CLOSESOCKET(client);

                continue;
            }

            string body = readFile(path);

            if (body.empty()) {

                sendResponse(
                    client,
                    "500 Internal Server Error",
                    "text/plain",
                    "Could not read " + path +
                    ". Run server from project root folder."
                );
            }
            else {

                sendResponse(
                    client,
                    "200 OK",
                    type,
                    body
                );
            }
        }

        CLOSESOCKET(client);
    }

#ifdef _WIN32
    WSACleanup();
#endif

    CLOSESOCKET(server);

    return 0;
}
