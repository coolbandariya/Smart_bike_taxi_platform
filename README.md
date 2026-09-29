# Smart Bike Taxi Platform

A console-based C++17 DSA project based on the submitted project synopsis.

## Current milestone

- City modeled as an undirected weighted graph using an adjacency list.
- Dijkstra's algorithm computes a shortest route and distance.
- A starter fare estimate is calculated as `Rs. 20 + Rs. 10 × distance_km`.
- The city map and fare rates are sample values for demonstration, not live data.

## Build and run

### Windows (MinGW g++)
Open PowerShell in this folder:

```powershell
g++ -std=c++17 -Wall -Wextra -pedantic main.cpp -o smart_bike_taxi.exe
.\smart_bike_taxi.exe
```

### Linux / macOS
```bash
g++ -std=c++17 -Wall -Wextra -pedantic main.cpp -o smart_bike_taxi
./smart_bike_taxi
```

Enter a location exactly as it appears in the displayed list.

## Planned modules

1. Rider, Driver, and Ride models
2. Custom hash table for rider/driver lookup
3. Ride request FIFO queue
4. Min-heap for driver selection
5. Booking workflow and ride history
6. Module tests and final documentation

## DSA notes

Dijkstra's algorithm is used because road weights are non-negative. With the priority queue used here, the typical time complexity is `O((V + E) log V)` for a sparse graph. The graph stores each two-way road in both directions.

## Limitations

This is an academic simulation. It does not use GPS, a map service, a database, a payment provider, or a real driver network.
