# Algorithm behavior and complexity

This repository is an academic simulation, not a live ride-hailing service.

| Component | Responsibility | Expected complexity |
| --- | --- | --- |
| Weighted adjacency list | Store the illustrative road network | O(V + E) space |
| Dijkstra with a binary heap | Find a shortest path over non-negative edge weights | O((V + E) log V) |
| Linear-probing hash table | Find rider/driver records by key | Average O(1), worst O(n) |
| FIFO request queue | Preserve request order | O(1) enqueue/dequeue |
| Min-heap driver selection | Select the eligible driver with the lowest pickup-route cost | O(log D) per heap operation |

Here, V is the number of locations, E is the number of roads, and D is the number of eligible drivers. Hash-table performance depends on its load factor and collision pattern.

## Behavioral invariants to preserve
- A driver assigned to an active ride must not be dispatched to another ride.
- Cancelling a pending request must not reorder the remaining queue.
- An unreachable pickup or destination must not produce a fabricated route.
- Fare values are simulation estimates, not real quotes.
- Persisted state should be validated before it is trusted.

When changing these algorithms, add regression tests for the boundary case as well as the normal path. Run CMake build and CTest before merging.