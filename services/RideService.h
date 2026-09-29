#ifndef SMART_BIKE_TAXI_RIDE_SERVICE_H
#define SMART_BIKE_TAXI_RIDE_SERVICE_H

#include <functional>
#include <queue>
#include <string>
#include <vector>

#include "graph/Graph.h"
#include "models/Models.h"
#include "structures/HashTable.h"

namespace ride_service {
constexpr double BASE_FARE = 20.0;
constexpr double FARE_PER_KM = 10.0;

enum class DispatchStatus {
    NoRequest,
    NoAvailableDriver,
    Dispatched,
    InvalidRequest
};

struct DispatchResult {
    DispatchStatus status = DispatchStatus::NoRequest;
    Ride ride;
    std::string message;
};

// Dispatches the FIFO request to the reachable available driver with the
// shortest route to pickup. The console app simulates immediate completion.
inline DispatchResult dispatchNext(
    const HashTable<Rider>& riders, HashTable<Driver>& drivers,
    const std::vector<std::string>& driverIds, const Graph& city,
    std::queue<RideRequest>& requests, std::vector<Ride>& history,
    int& nextRideId) {
    if (requests.empty()) return {DispatchStatus::NoRequest, {}, "No pending ride requests."};

    const RideRequest request = requests.front();
    const Rider* rider = riders.find(request.riderId);
    const auto trip = city.shortestPath(request.pickup, request.destination);
    if (!rider || !trip.reachable || request.requestId <= 0 || nextRideId <= 0) {
        return {DispatchStatus::InvalidRequest, {}, "Request has invalid rider, route, or ID."};
    }

    using Candidate = std::pair<double, std::string>;
    std::priority_queue<Candidate, std::vector<Candidate>, std::greater<Candidate>> candidates;
    for (const auto& id : driverIds) {
        const Driver* driver = drivers.find(id);
        if (!driver || !driver->available) continue;
        const auto route = city.shortestPath(driver->location, request.pickup);
        if (route.reachable) candidates.push({route.distance, id});
    }
    if (candidates.empty()) {
        return {DispatchStatus::NoAvailableDriver, {}, "No reachable available driver."};
    }

    Driver* driver = drivers.find(candidates.top().second);
    if (!driver) return {DispatchStatus::InvalidRequest, {}, "Selected driver no longer exists."};

    Ride ride;
    ride.rideId = nextRideId;
    ride.riderId = request.riderId;
    ride.driverId = driver->id;
    ride.pickup = request.pickup;
    ride.destination = request.destination;
    ride.distanceKm = trip.distance;
    ride.fare = BASE_FARE + trip.distance * FARE_PER_KM;
    ride.route = trip.path;
    ride.status = "Completed (simulated)";

    // Commit state only after all validation and route selection succeeded.
    requests.pop();
    ++nextRideId;
    driver->available = false;
    driver->location = request.destination;
    driver->available = true;
    history.push_back(ride);
    return {DispatchStatus::Dispatched, ride, "Ride dispatched and completed (simulation)."};
}
} // namespace ride_service

#endif
