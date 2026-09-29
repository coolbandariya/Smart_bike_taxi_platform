#ifndef SMART_BIKE_TAXI_STORAGE_H
#define SMART_BIKE_TAXI_STORAGE_H

#include <cstdio>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <queue>
#include <string>
#include <vector>

#include "models/Models.h"
#include "structures/HashTable.h"

// Simple versioned, quoted-text persistence for this single-user console demo.
namespace storage {
inline const char* fileName() { return "smart_bike_taxi_data.txt"; }

inline bool save(const HashTable<Rider>& riders,
                 const std::vector<std::string>& riderIds,
                 const HashTable<Driver>& drivers,
                 const std::vector<std::string>& driverIds,
                 const std::queue<RideRequest>& pending,
                 const std::vector<Ride>& history,
                 int nextRequestId, int nextRideId) {
    const std::string temp = std::string(fileName()) + ".tmp";
    std::ofstream out(temp, std::ios::trunc);
    if (!out) return false;
    out << "SMART_BIKE_TAXI_V1\n";
    out << riderIds.size() << ' ' << driverIds.size() << ' '
        << pending.size() << ' ' << history.size() << ' '
        << nextRequestId << ' ' << nextRideId << '\n';
    for (const auto& id : riderIds) {
        const Rider* r = riders.find(id);
        if (!r) return false;
        out << std::quoted(r->id) << ' ' << std::quoted(r->name) << ' '
            << std::quoted(r->phone) << '\n';
    }
    for (const auto& id : driverIds) {
        const Driver* d = drivers.find(id);
        if (!d) return false;
        out << std::quoted(d->id) << ' ' << std::quoted(d->name) << ' '
            << std::quoted(d->phone) << ' ' << std::quoted(d->location) << ' '
            << d->available << '\n';
    }
    auto requests = pending;
    while (!requests.empty()) {
        const auto& r = requests.front();
        out << r.requestId << ' ' << std::quoted(r.riderId) << ' '
            << std::quoted(r.pickup) << ' ' << std::quoted(r.destination) << '\n';
        requests.pop();
    }
    for (const auto& r : history) {
        out << r.rideId << ' ' << std::quoted(r.riderId) << ' '
            << std::quoted(r.driverId) << ' ' << std::quoted(r.pickup) << ' '
            << std::quoted(r.destination) << ' ' << std::setprecision(17)
            << r.distanceKm << ' ' << r.fare << ' ' << std::quoted(r.status)
            << ' ' << r.route.size();
        for (const auto& stop : r.route) out << ' ' << std::quoted(stop);
        out << '\n';
    }
    out.flush();
    if (!out) {
        out.close();
        std::remove(temp.c_str());
        return false;
    }
    out.close();
    if (!out) {
        std::remove(temp.c_str());
        return false;
    }

    // Keep the previous snapshot until the new file is safely in place.
    // The backup also lets us recover if the second rename fails.
    const std::string backup = std::string(fileName()) + ".bak";
    const bool hadPrevious = std::rename(fileName(), backup.c_str()) == 0;
    if (!hadPrevious) {
        // A missing destination is expected on the first save. If it exists
        // but cannot be moved, do not risk replacing it.
        std::ifstream existing(fileName());
        if (existing.good()) {
            std::remove(temp.c_str());
            return false;
        }
    }

    if (std::rename(temp.c_str(), fileName()) != 0) {
        if (hadPrevious) std::rename(backup.c_str(), fileName());
        std::remove(temp.c_str());
        return false;
    }
    if (hadPrevious) std::remove(backup.c_str());
    return true;
}

inline bool load(HashTable<Rider>& riders, std::vector<std::string>& riderIds,
                 HashTable<Driver>& drivers, std::vector<std::string>& driverIds,
                 std::queue<RideRequest>& pending, std::vector<Ride>& history,
                 int& nextRequestId, int& nextRideId) {
    std::ifstream in(fileName());
    if (!in) return true; // First run.

    // Parse into temporary state. A malformed file must not partially mutate
    // the live application state.
    HashTable<Rider> parsedRiders;
    HashTable<Driver> parsedDrivers;
    std::vector<std::string> parsedRiderIds, parsedDriverIds;
    std::queue<RideRequest> parsedPending;
    std::vector<Ride> parsedHistory;
    int parsedNextRequestId = 1, parsedNextRideId = 1;

    std::string magic;
    if (!std::getline(in, magic) || magic != "SMART_BIKE_TAXI_V1") return false;
    std::size_t nr = 0, nd = 0, np = 0, nh = 0;
    if (!(in >> nr >> nd >> np >> nh >> parsedNextRequestId >> parsedNextRideId) ||
        nr > 100 || nd > 100 || np > 10000 || nh > 100000 ||
        parsedNextRequestId < 1 || parsedNextRideId < 1) return false;

    int maxRequestId = 0, maxRideId = 0;
    for (std::size_t i = 0; i < nr; ++i) {
        Rider r;
        if (!(in >> std::quoted(r.id) >> std::quoted(r.name) >> std::quoted(r.phone)) ||
            r.id.empty() || r.name.empty() || r.phone.empty() ||
            parsedRiders.find(r.id) || !parsedRiders.insert(r.id, r)) return false;
        parsedRiderIds.push_back(r.id);
    }
    for (std::size_t i = 0; i < nd; ++i) {
        Driver d;
        int available = 0;
        if (!(in >> std::quoted(d.id) >> std::quoted(d.name) >> std::quoted(d.phone)
                 >> std::quoted(d.location) >> available) ||
            d.id.empty() || d.name.empty() || d.phone.empty() || d.location.empty() ||
            (available != 0 && available != 1) || parsedDrivers.find(d.id)) return false;
        d.available = available == 1;
        if (!parsedDrivers.insert(d.id, d)) return false;
        parsedDriverIds.push_back(d.id);
    }
    for (std::size_t i = 0; i < np; ++i) {
        RideRequest r;
        if (!(in >> r.requestId >> std::quoted(r.riderId) >> std::quoted(r.pickup)
                 >> std::quoted(r.destination)) || r.requestId < 1 ||
            r.riderId.empty() || r.pickup.empty() || r.destination.empty() ||
            r.pickup == r.destination || !parsedRiders.find(r.riderId)) return false;
        parsedPending.push(r);
        if (r.requestId > maxRequestId) maxRequestId = r.requestId;
    }
    for (std::size_t i = 0; i < nh; ++i) {
        Ride r;
        std::size_t routeSize = 0;
        if (!(in >> r.rideId >> std::quoted(r.riderId) >> std::quoted(r.driverId)
                 >> std::quoted(r.pickup) >> std::quoted(r.destination)
                 >> r.distanceKm >> r.fare >> std::quoted(r.status) >> routeSize) ||
            r.rideId < 1 || routeSize == 0 || routeSize > 1000 ||
            r.riderId.empty() || r.driverId.empty() || r.pickup.empty() ||
            r.destination.empty() || r.pickup == r.destination ||
            !std::isfinite(r.distanceKm) || r.distanceKm <= 0.0 ||
            !std::isfinite(r.fare) || r.fare < 0.0 || r.status.empty() ||
            !parsedRiders.find(r.riderId) || !parsedDrivers.find(r.driverId)) return false;
        for (std::size_t j = 0; j < routeSize; ++j) {
            std::string stop;
            if (!(in >> std::quoted(stop)) || stop.empty()) return false;
            r.route.push_back(stop);
        }
        if (r.route.front() != r.pickup || r.route.back() != r.destination)
            return false;
        parsedHistory.push_back(r);
        if (r.rideId > maxRideId) maxRideId = r.rideId;
    }
    in >> std::ws;
    if (!in.eof() || parsedNextRequestId <= maxRequestId ||
        parsedNextRideId <= maxRideId) return false;

    riders = parsedRiders;
    riderIds = std::move(parsedRiderIds);
    drivers = parsedDrivers;
    driverIds = std::move(parsedDriverIds);
    pending = std::move(parsedPending);
    history = std::move(parsedHistory);
    nextRequestId = parsedNextRequestId;
    nextRideId = parsedNextRideId;
    return true;
}
} // namespace storage
#endif
