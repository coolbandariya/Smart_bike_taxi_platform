#include <cassert>
#include <cmath>
#include <limits>
#include <iostream>

#include "graph/Graph.h"
#include "models/Models.h"
#include "structures/HashTable.h"
#include "storage/Storage.h"
#include <cstdio>
#include <queue>
#include <vector>

int main() {
    Graph graph;
    assert(graph.addLocation("A"));
    assert(graph.addLocation("B"));
    assert(graph.addLocation("C"));
    assert(!graph.addLocation("A"));
    assert(graph.addRoad("A", "B", 4.0));
    assert(graph.addRoad("B", "C", 3.0));
    assert(graph.addRoad("A", "C", 10.0));

    const auto route = graph.shortestPath("A", "C");
    assert(route.reachable);
    assert(std::abs(route.distance - 7.0) < 1e-9);
    assert(route.path.size() == 3);
    assert(route.path.front() == "A" && route.path.back() == "C");
    assert(!graph.shortestPath("A", "missing").reachable);
    assert(!graph.addRoad("A", "missing", 1.0));
    assert(!graph.addRoad("A", "C", -1.0));
    assert(!graph.addRoad("A", "C", std::numeric_limits<double>::infinity()));
    assert(!graph.addRoad("A", "C", std::numeric_limits<double>::quiet_NaN()));
    assert(graph.addRoad("A", "A", 0.0));

    HashTable<Rider> riders(5);
    Rider rider{"R1", "Test Rider", "0000000000"};
    assert(riders.insert(rider.id, rider));
    assert(riders.size() == 1);
    assert(riders.find("R1") != nullptr);
    assert(riders.find("R1")->name == "Test Rider");
    assert(riders.find("missing") == nullptr);
    assert(!riders.insert("", rider));

    Rider updated{"R1", "Updated Rider", "1111111111"};
    HashTable<Rider> tiny(1);
    assert(tiny.insert("only", rider));
    assert(!tiny.insert("second", updated));

    assert(riders.insert(updated.id, updated));
    assert(riders.size() == 1);
    assert(riders.find("R1")->name == "Updated Rider");

    std::cout << "All tests passed.\n";
    return 0;
}
