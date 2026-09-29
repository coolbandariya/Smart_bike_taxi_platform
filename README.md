# Smart Bike Taxi Platform

A console-based C++17 DSA project. It is an academic simulation using a small illustrative Noida road map.

## Implemented features

- **Weighted graph + Dijkstra:** finds the shortest route between known locations.
- **Rider and driver registration:** stores records in custom hash tables during the program session.
- **FIFO ride queue:** ride requests are processed in arrival order.
- **Min-heap driver matching:** selects the available driver with the shortest route to the pickup.
- **Fare estimate:** sample formula `Rs. 20 + Rs. 10 × distance_km`.
- **Ride history:** records simulated completed rides for the current session.

## Build and run

### Windows (MinGW g++)
Open PowerShell in the project folder:

```powershell
g++ -std=c++17 -Wall -Wextra -pedantic main.cpp -o smart_bike_taxi.exe
.\smart_bike_taxi.exe
```

### Linux / macOS

```bash
g++ -std=c++17 -Wall -Wextra -pedantic main.cpp -o smart_bike_taxi
./smart_bike_taxi
```

Enter location names exactly as shown by the application.

## Run tests

The tests cover shortest-path behavior and the custom hash table.

### Windows

```powershell
g++ -std=c++17 -Wall -Wextra -pedantic tests.cpp -o tests.exe
.\tests.exe
```

### Linux / macOS

```bash
g++ -std=c++17 -Wall -Wextra -pedantic tests.cpp -o tests
./tests
```

## Project structure

- `main.cpp` — interactive menu and booking workflow
- `graph/Graph.h` — adjacency-list graph and Dijkstra implementation
- `models/Models.h` — rider, driver, request, and ride data models
- `structures/HashTable.h` — custom open-addressing hash table
- `tests.cpp` — basic assertions for graph and hash table

## DSA overview

| Component | Data structure / algorithm | Purpose |
|---|---|---|
| City map | Adjacency list | Stores roads and distances |
| Route finding | Dijkstra + min-priority queue | Finds shortest distance |
| Rider/driver lookup | Linear-probing hash table | Average-case constant-time lookup |
| Pending requests | FIFO queue | Preserves request order |
| Driver matching | Min-heap | Chooses the closest available driver |

For a sparse graph, Dijkstra with a binary heap typically runs in `O((V + E) log V)` time.

## Current limitations

- The map and road distances are sample data, not live map data.
- Rider and driver records exist only in memory and disappear when the program exits.
- Phone numbers are demonstration inputs; no OTP or identity verification is performed.
- A dispatched ride is marked completed immediately to demonstrate the workflow.
- There is no GPS, live driver network, payment integration, or persistent database.
