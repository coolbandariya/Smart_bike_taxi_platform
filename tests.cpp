#include <cassert>
#include <cmath>
#include <cstdio>
#include <iostream>
#include <limits>
#include <queue>
#include <string>
#include <vector>

#include "graph/Graph.h"
#include "models/Models.h"
#include "storage/Storage.h"
#include "structures/HashTable.h"

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
    assert(route.reachable && std::abs(route.distance - 7.0) < 1e-9);
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
    assert(riders.insert(updated.id, updated));
    assert(riders.size() == 1);
    assert(riders.find("R1")->name == "Updated Rider");
    HashTable<Rider> tiny(1);
    assert(tiny.insert("only", rider));
    assert(!tiny.insert("second", updated));

    HashTable<Rider> savedRiders;
    HashTable<Driver> savedDrivers;
    std::vector<std::string> riderIds{"R1"}, driverIds{"D1"};
    assert(savedRiders.insert("R1", updated));
    Driver driver{"D1", "Test Driver", "2222222222", "A", true};
    assert(savedDrivers.insert("D1", driver));
    std::queue<RideRequest> pending;
    pending.push(RideRequest{7, "R1", "A", "C"});
    std::vector<Ride> rides;
    Ride savedRide;
    savedRide.rideId = 4;
    savedRide.riderId = "R1";
    savedRide.driverId = "D1";
    savedRide.pickup = "A";
    savedRide.destination = "C";
    savedRide.distanceKm = 7.0;
    savedRide.fare = 90.0;
    savedRide.route = {"A", "B", "C"};
    savedRide.status = "Completed (simulated)";
    rides.push_back(savedRide);
    assert(storage::save(savedRiders, riderIds, savedDrivers, driverIds,
                         pending, rides, 8, 5));

    HashTable<Rider> loadedRiders;
    HashTable<Driver> loadedDrivers;
    std::vector<std::string> loadedRiderIds, loadedDriverIds;
    std::queue<RideRequest> loadedPending;
    std::vector<Ride> loadedRides;
    int nextRequest = 1, nextRide = 1;
    assert(storage::load(loadedRiders, loadedRiderIds, loadedDrivers,
                         loadedDriverIds, loadedPending, loadedRides,
                         nextRequest, nextRide));
    assert(loadedRiders.find("R1") &&
           loadedRiders.find("R1")->name == "Updated Rider");
    assert(loadedDrivers.find("D1") &&
           loadedDrivers.find("D1")->location == "A");
    assert(loadedPending.size() == 1 && loadedPending.front().requestId == 7);
    assert(loadedRides.size() == 1 && loadedRides.front().route.size() == 3);
    assert(nextRequest == 8 && nextRide == 5);
    std::remove(storage::fileName());

    std::cout << "All tests passed.\n";
    return 0;
}
