#include <iomanip>
#include <limits>
#include <iostream>
#include <queue>
#include <string>
#include <vector>

#include "graph/Graph.h"
#include "models/Models.h"
#include "structures/HashTable.h"
#include "storage/Storage.h"

namespace {
constexpr double BASE_FARE = 20.0;
constexpr double FARE_PER_KM = 10.0;

std::string readLine(const std::string& prompt) {
    std::cout << prompt;
    std::string value;
    std::getline(std::cin, value);
    return value;
}

int readChoice() {
    const std::string input = readLine("\nChoose an option: ");
    try {
        std::size_t used = 0;
        const int value = std::stoi(input, &used);
        if (used == input.size()) return value;
    } catch (...) {
    }
    return -1;
}

void printRoute(const std::vector<std::string>& route) {
    for (std::size_t i = 0; i < route.size(); ++i) {
        if (i) std::cout << " -> ";
        std::cout << route[i];
    }
}

void registerRider(HashTable<Rider>& riders, std::vector<std::string>& riderIds) {
    Rider rider;
    rider.id = readLine("Rider ID: ");
    if (rider.id.empty() || riders.find(rider.id)) {
        std::cout << "ID is empty or already registered.\n";
        return;
    }
    rider.name = readLine("Rider name: ");
    rider.phone = readLine("Phone (demo only): ");
    if (rider.name.empty() || rider.phone.empty()) {
        std::cout << "Name and phone cannot be empty.\n";
        return;
    }
    if (!riders.insert(rider.id, rider)) {
        std::cout << "Rider table is full.\n";
        return;
    }
    std::cout << "Rider registered successfully.\n";
}

void registerDriver(HashTable<Driver>& drivers,
                    std::vector<std::string>& driverIds,
                    const Graph& city) {
    Driver driver;
    driver.id = readLine("Driver ID: ");
    if (driver.id.empty() || drivers.find(driver.id)) {
        std::cout << "ID is empty or already registered.\n";
        return;
    }
    driver.name = readLine("Driver name: ");
    driver.phone = readLine("Phone (demo only): ");
    driver.location = readLine("Current location (use listed name): ");
    if (driver.name.empty() || driver.phone.empty() ||
        !city.shortestPath(driver.location, driver.location).reachable) {
        std::cout << "Invalid driver details or location.\n";
        return;
    }
    if (!drivers.insert(driver.id, driver)) {
        std::cout << "Driver table is full.\n";
        return;
    }
    driverIds.push_back(driver.id);
    std::cout << "Driver registered and marked available.\n";
}

void requestRide(const HashTable<Rider>& riders, const Graph& city,
                 std::queue<RideRequest>& requests, int& nextRequestId) {
    RideRequest request;
    request.riderId = readLine("Rider ID: ");
    if (!riders.find(request.riderId)) {
        std::cout << "Rider not found. Register the rider first.\n";
        return;
    }
    request.pickup = readLine("Pickup location: ");
    request.destination = readLine("Destination: ");
    const auto route = city.shortestPath(request.pickup, request.destination);
    if (!route.reachable) {
        std::cout << "Invalid location or no route available.\n";
        return;
    }
    request.requestId = nextRequestId++;
    requests.push(request);
    std::cout << "Ride request #" << request.requestId
              << " added to the FIFO queue.\n";
    std::cout << "Estimated distance: " << std::fixed << std::setprecision(2)
              << route.distance << " km; estimated fare: Rs. "
              << BASE_FARE + route.distance * FARE_PER_KM << "\n";
}

void dispatchNext(const HashTable<Rider>& riders, HashTable<Driver>& drivers,
                  const std::vector<std::string>& driverIds, const Graph& city,
                  std::queue<RideRequest>& requests, std::vector<Ride>& history,
                  int& nextRideId) {
    if (requests.empty()) {
        std::cout << "No pending ride requests.\n";
        return;
    }

    const RideRequest request = requests.front();
    requests.pop();

    using Candidate = std::pair<double, std::string>;
    std::priority_queue<Candidate, std::vector<Candidate>,
                        std::greater<Candidate>> nearestDrivers;

    for (const auto& driverId : driverIds) {
        const Driver* driver = drivers.find(driverId);
        if (!driver || !driver->available) continue;
        const auto routeToPickup = city.shortestPath(driver->location, request.pickup);
        if (routeToPickup.reachable) {
            nearestDrivers.push({routeToPickup.distance, driverId});
        }
    }

    if (nearestDrivers.empty()) {
        requests.push(request);
        std::cout << "No available driver. Request #" << request.requestId
                  << " remains queued.\n";
        return;
    }

    const std::string driverId = nearestDrivers.top().second;
    Driver* driver = drivers.find(driverId);
    const auto trip = city.shortestPath(request.pickup, request.destination);
    if (!driver || !trip.reachable) {
        requests.push(request);
        std::cout << "Could not dispatch this request; it was requeued.\n";
        return;
    }

    driver->available = false;
    Ride ride;
    ride.rideId = nextRideId++;
    ride.riderId = request.riderId;
    ride.driverId = driverId;
    ride.pickup = request.pickup;
    ride.destination = request.destination;
    ride.distanceKm = trip.distance;
    ride.fare = BASE_FARE + trip.distance * FARE_PER_KM;
    ride.route = trip.path;
    ride.status = "Completed (simulated)";

    // The console simulation completes the trip immediately.
    driver->location = request.destination;
    driver->available = true;
    history.push_back(ride);

    std::cout << "\nRide #" << ride.rideId << " dispatched and completed (simulation).\n";
    std::cout << "Rider: " << riders.find(request.riderId)->name
              << " | Driver: " << driver->name << " (" << driver->id << ")\n";
    std::cout << "Route: ";
    printRoute(ride.route);
    std::cout << "\nDistance: " << std::fixed << std::setprecision(2)
              << ride.distanceKm << " km | Fare: Rs. " << ride.fare << "\n";
}

void cancelRequest(std::queue<RideRequest>& requests) {
    if (requests.empty()) {
        std::cout << "No pending requests to cancel.\n";
        return;
    }
    const std::string raw = readLine("Request ID to cancel: ");
    int id = -1;
    try {
        std::size_t used = 0;
        id = std::stoi(raw, &used);
        if (used != raw.size()) id = -1;
    } catch (...) {
        id = -1;
    }
    std::queue<RideRequest> retained;
    bool removed = false;
    while (!requests.empty()) {
        RideRequest request = requests.front();
        requests.pop();
        if (request.requestId == id && !removed) {
            removed = true;
        } else {
            retained.push(request);
        }
    }
    requests.swap(retained);
    std::cout << (removed ? "Request cancelled.\n" : "Request ID not found.\n");
}

void showDrivers(const HashTable<Driver>& drivers,
                 const std::vector<std::string>& driverIds) {
    if (driverIds.empty()) {
        std::cout << "No drivers registered.\n";
        return;
    }
    for (const auto& id : driverIds) {
        const Driver* driver = drivers.find(id);
        if (driver) {
            std::cout << id << " | " << driver->name << " | "
                      << driver->location << " | "
                      << (driver->available ? "Available" : "Busy") << '\n';
        }
    }
}

void toggleDriverAvailability(HashTable<Driver>& drivers,
                                const std::vector<std::string>& driverIds) {
    if (driverIds.empty()) {
        std::cout << "No drivers registered.\\n";
        return;
    }
    const std::string id = readLine("Driver ID: ");
    Driver* driver = drivers.find(id);
    if (!driver) {
        std::cout << "Driver not found.\\n";
        return;
    }
    driver->available = !driver->available;
    std::cout << "Driver is now "
              << (driver->available ? "Available" : "Unavailable") << ".\\n";
}

void showHistory(const std::vector<Ride>& history) {
    if (history.empty()) {
        std::cout << "No completed rides yet.\n";
        return;
    }
    for (const Ride& ride : history) {
        std::cout << "\nRide #" << ride.rideId << " | Rider: " << ride.riderId
                  << " | Driver: " << ride.driverId << "\n"
                  << ride.pickup << " -> " << ride.destination
                  << " | " << std::fixed << std::setprecision(2)
                  << ride.distanceKm << " km | Rs. " << ride.fare
                  << " | " << ride.status << "\n";
    }
}
} // namespace

int main() {
    Graph city;
    city.addLocation("JIIT 128");
    city.addLocation("Sector 62");
    city.addLocation("Botanical Garden");
    city.addLocation("Noida City Centre");
    city.addLocation("Sector 18");

    city.addRoad("JIIT 128", "Sector 62", 8.0);
    city.addRoad("Sector 62", "Botanical Garden", 10.0);
    city.addRoad("Botanical Garden", "Noida City Centre", 6.0);
    city.addRoad("Noida City Centre", "Sector 18", 7.0);
    city.addRoad("Sector 62", "Noida City Centre", 12.0);
    city.addRoad("JIIT 128", "Botanical Garden", 22.0);
    city.addRoad("Botanical Garden", "Sector 18", 15.0);

    HashTable<Rider> riders;
    std::vector<std::string> riderIds;
    HashTable<Driver> drivers;
    std::vector<std::string> driverIds;
    std::queue<RideRequest> requests;
    std::vector<Ride> history;
    int nextRequestId = 1;
    int nextRideId = 1;
    if (!storage::load(riders, riderIds, drivers, driverIds, requests, history,
                       nextRequestId, nextRideId)) {
        std::cerr << "Could not load saved data. Check smart_bike_taxi_data.txt.\\n";
        return 1;
    }

    std::cout << "====================================\n"
              << "      SMART BIKE TAXI PLATFORM\n"
              << "====================================\n"
              << "Academic console simulation | sample Noida map\n";

    bool running = true;
    while (running) {
        std::cout << "\n1. Show locations\n"
                  << "2. Register rider\n"
                  << "3. Register driver\n"
                  << "4. Request a ride\n"
                  << "5. Dispatch next request\n"
                  << "6. Show ride history\n"
                  << "7. Show pending request count\n"
                  << "8. Cancel a pending request\n"
                  << "9. Show drivers\n"
                  << "10. Toggle driver availability\n"
                  << "0. Exit\n";
        switch (readChoice()) {
            case 1: city.printLocations(); break;
            case 2: registerRider(riders, riderIds); break;
            case 3: registerDriver(drivers, driverIds, city); break;
            case 4: requestRide(riders, city, requests, nextRequestId); break;
            case 5: dispatchNext(riders, drivers, driverIds, city,
                                 requests, history, nextRideId); break;
            case 6: showHistory(history); break;
            case 7: std::cout << "Pending requests: " << requests.size() << "\n"; break;
            case 8: cancelRequest(requests); break;
            case 9: showDrivers(drivers, driverIds); break;
            case 10: toggleDriverAvailability(drivers, driverIds); break;
            case 0: running = false; break;
            default: std::cout << "Invalid choice. Enter a number from the menu.\n";
        }
    }
    std::cout << "Thank you for using the Smart Bike Taxi Platform.\n";
    return 0;
}
