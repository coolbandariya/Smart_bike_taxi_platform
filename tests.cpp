#include <cmath>
#include <cstdio>
#include <iostream>
#include <limits>
#include <queue>
#include <stdexcept>
#include <string>
#include <vector>

#include "graph/Graph.h"
#include "models/Models.h"
#include "storage/Storage.h"
#include "structures/HashTable.h"

namespace {
void check(bool condition, const std::string& message) {
    if (!condition) throw std::runtime_error(message);
}

void testGraph() {
    Graph graph;
    check(graph.addLocation("A"), "add A");
    check(graph.addLocation("B"), "add B");
    check(graph.addLocation("C"), "add C");
    check(!graph.addLocation("A"), "duplicate location rejected");
    check(graph.addRoad("A", "B", 4.0), "add A-B");
    check(graph.addRoad("B", "C", 3.0), "add B-C");
    check(graph.addRoad("A", "C", 10.0), "add A-C");

    const auto route = graph.shortestPath("A", "C");
    check(route.reachable, "A to C is reachable");
    check(std::abs(route.distance - 7.0) < 1e-9, "Dijkstra distance");
    check(route.path.size() == 3 && route.path.front() == "A" &&
              route.path.back() == "C",
          "Dijkstra route reconstruction");
    check(!graph.shortestPath("A", "missing").reachable, "unknown destination");
    check(!graph.addRoad("A", "missing", 1.0), "unknown road endpoint");
    check(!graph.addRoad("A", "C", -1.0), "negative road rejected");
    check(!graph.addRoad("A", "C", std::numeric_limits<double>::infinity()),
          "infinite road rejected");
    check(!graph.addRoad("A", "C", std::numeric_limits<double>::quiet_NaN()),
          "NaN road rejected");
    check(graph.addRoad("A", "A", 0.0), "zero-length self road accepted");
}

void testHashTable() {
    HashTable<Rider> riders(5);
    const Rider first{"R1", "Test Rider", "0000000000"};
    check(riders.insert(first.id, first), "insert rider");
    check(riders.size() == 1, "hash table size after insert");
    check(riders.find("R1") != nullptr, "find existing rider");
    check(riders.find("R1")->name == "Test Rider", "stored rider value");
    check(riders.find("missing") == nullptr, "missing key returns null");
    check(!riders.insert("", first), "empty key rejected");

    const Rider updated{"R1", "Updated Rider", "1111111111"};
    check(riders.insert(updated.id, updated), "update existing key");
    check(riders.size() == 1, "update does not increase size");
    check(riders.find("R1")->name == "Updated Rider", "updated value");

    HashTable<Rider> tiny(1);
    check(tiny.insert("only", first), "insert into one-slot table");
    check(!tiny.insert("second", updated), "full table rejects new key");
    HashTable<Rider> zeroCapacity(0);
    check(zeroCapacity.insert("one", first), "zero capacity normalizes safely");
}

void testPersistence() {
    HashTable<Rider> savedRiders;
    HashTable<Driver> savedDrivers;
    const Rider rider{"R1", "Test Rider", "demo-phone"};
    const Driver driver{"D1", "Test Driver", "demo-phone", "A", true};
    check(savedRiders.insert(rider.id, rider), "prepare saved rider");
    check(savedDrivers.insert(driver.id, driver), "prepare saved driver");

    std::vector<std::string> riderIds{"R1"};
    std::vector<std::string> driverIds{"D1"};
    std::queue<RideRequest> pending;
    pending.push(RideRequest{7, "R1", "A", "C"});

    Ride ride;
    ride.rideId = 4;
    ride.riderId = "R1";
    ride.driverId = "D1";
    ride.pickup = "A";
    ride.destination = "C";
    ride.distanceKm = 7.0;
    ride.fare = 90.0;
    ride.route = {"A", "B", "C"};
    ride.status = "Completed (simulated)";
    const std::vector<Ride> history{ride};

    check(storage::save(savedRiders, riderIds, savedDrivers, driverIds,
                        pending, history, 8, 5),
          "save persistence fixture");

    HashTable<Rider> loadedRiders;
    HashTable<Driver> loadedDrivers;
    std::vector<std::string> loadedRiderIds, loadedDriverIds;
    std::queue<RideRequest> loadedPending;
    std::vector<Ride> loadedHistory;
    int nextRequest = 1;
    int nextRide = 1;
    const bool loaded = storage::load(
        loadedRiders, loadedRiderIds, loadedDrivers, loadedDriverIds,
        loadedPending, loadedHistory, nextRequest, nextRide);
    std::remove(storage::fileName());
    std::remove((std::string(storage::fileName()) + ".tmp").c_str());

    check(loaded, "load persistence fixture");
    check(loadedRiders.find("R1") != nullptr, "restored rider exists");
    check(loadedRiders.find("R1")->name == "Test Rider", "restored rider fields");
    check(loadedDrivers.find("D1") != nullptr, "restored driver exists");
    check(loadedDrivers.find("D1")->location == "A", "restored driver fields");
    check(loadedPending.size() == 1 && loadedPending.front().requestId == 7,
          "restored pending request");
    check(loadedHistory.size() == 1 && loadedHistory.front().route.size() == 3,
          "restored ride and route");
    check(nextRequest == 8 && nextRide == 5, "restored ID counters");
    check(loadedRiderIds.size() == 1 && loadedDriverIds.size() == 1,
          "restored record indexes");
}
}  // namespace

int main() {
    try {
        testGraph();
        testHashTable();
        testPersistence();
        std::cout << "All tests passed.\n";
        return 0;
    } catch (const std::exception& error) {
        std::cerr << "Test failure: " << error.what() << '\n';
        return 1;
    }
}
