# Smart City Route Finder — C++ DSA Web Project

A small full-stack college project. The browser UI is built with HTML/CSS/JavaScript; the route algorithms and HTTP server are implemented in C++17.

## DSA concepts used
- **Graph (adjacency list):** locations are vertices; roads are weighted, undirected edges.
- **Dijkstra:** minimum total distance using a min-priority queue.
- **BFS:** finds a route with the fewest road segments (not necessarily the least distance).
- **DFS:** depth-first traversal; finds a valid path but does not guarantee an optimal path.
- **Parent array:** reconstructs the selected route from destination to source.

## Requirements
- C++17 compiler (MinGW-w64 g++, MSVC, or g++)
- CMake 3.16+ (optional; direct compiler command also works)
- A modern browser

## Run on Windows (MinGW)
1. Extract the ZIP.
2. Open the project folder in VS Code.
3. Open Terminal in the project root.
4. Compile:
   ```powershell
   g++ -std=c++17 server.cpp -o server.exe -lws2_32
   ```
5. Run:
   ```powershell
   .\server.exe
   ```
6. Open **http://localhost:8080** in your browser.

If port 8080 is busy, run `.\server.exe 8090` and open `http://localhost:8090`.

## Run on Linux / macOS
```bash
g++ -std=c++17 server.cpp -o server
./server
```
Then open http://localhost:8080.

## Run with CMake
```bash
cmake -S . -B build
cmake --build build
```
Run the generated executable from the project root because the server reads files from `public/`.

## Project structure
```text
smart_city_route_finder/
├── server.cpp
├── CMakeLists.txt
├── README.md
└── public/
    ├── index.html
    ├── styles.css
    └── app.js
```

## API
- `GET /api/locations` — returns the available location names.
- `GET /api/route?from=CGC%20University&to=Mohali%20Bus%20Stand&algorithm=dijkstra`
- Supported algorithm values: `dijkstra`, `bfs`, `dfs`.

## Important project assumptions
- The city graph and road distances are **sample/illustrative data**, not live maps or traffic information.
- Estimated travel time is a simple demo estimate of 3 minutes per kilometre; it is not a real navigation ETA.
- Dijkstra assumes non-negative edge weights. All sample road weights are positive.
- This is an educational local demo server, not production-ready internet-facing software.

## How to extend
Add a location to `CityGraph::names`, add its road connections in the constructor, and add a matching coordinate in `public/app.js`'s `positions` array. Keep the graph node order and UI coordinate order aligned.
