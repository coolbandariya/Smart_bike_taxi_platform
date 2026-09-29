#ifndef SMART_BIKE_TAXI_STORAGE_H
#define SMART_BIKE_TAXI_STORAGE_H

#include <cstdio>
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
    if (!out) return false;
    out.close();
    std::remove(fileName());
    return std::rename(temp.c_str(), fileName()) == 0;
}

inline bool load(HashTable<Rider>& riders, std::vector<std::string>& riderIds,
                 HashTable<Driver>& drivers, std::vector<std::string>& driverIds,
                 std::queue<RideRequest>& pending, std::vector<Ride>& history,
                 int& nextRequestId, int& nextRideId) {
    std::ifstream in(fileName());
    if (!in) return true; // First run.
    std::string magic;
    if (!std::getline(in, magic) || magic != "SMART_BIKE_TAXI_V1") return false;
    std::size_t nr = 0, nd = 0, np = 0, nh = 0;
    if (!(in >> nr >> nd >> np >> nh >> nextRequestId >> nextRideId) ||
        nr > 100 || nd > 100 || np > 10000 || nh > 100000 ||
        nextRequestId < 1 || nextRideId < 1) return false;
    for (std::size_t i = 0; i < nr; ++i) {
        Rider r;
        if (!(in >> std::quoted(r.id) >> std::quoted(r.name) >> std::quoted(r.phone)) ||
            r.id.empty() || r.name.empty() || r.phone.empty() || riders.find(r.id) ||
            !riders.insert(r.id, r)) return false;
        riderIds.push_back(r.id);
    }
    for (std::size_t i = 0; i < nd; ++i) {
        Driver d;
        if (!(in >> std::quoted(d.id) >> std::quoted(d.name) >> std::quoted(d.phone)
                 >> std::quoted(d.location) >> d.available) ||
            d.id.empty() || d.name.empty() || d.phone.empty() || d.location.empty() ||
            drivers.find(d.id) || !drivers.insert(d.id, d)) return false;
        driverIds.push_back(d.id);
    }
    for (std::size_t i = 0; i < np; ++i) {
        RideRequest r;
        if (!(in >> r.requestId >> std::quoted(r.riderId) >> std::quoted(r.pickup)
                 >> std::quoted(r.destination)) || r.requestId < 1 ||
            !riders.find(r.riderId)) return false;
        pending.push(r);
    }
    for (std::size_t i = 0; i < nh; ++i) {
        Ride r;
        std::size_t routeSize = 0;
        if (!(in >> r.rideId >> std::quoted(r.riderId) >> std::quoted(r.driverId)
                 >> std::quoted(r.pickup) >> std::quoted(r.destination)
                 >> r.distanceKm >> r.fare >> std::quoted(r.status) >> routeSize) ||
            r.rideId < 1 || routeSize > 1000 || !riders.find(r.riderId) ||
            !drivers.find(r.driverId)) return false;
        for (std::size_t j = 0; j < routeSize; ++j) {
            std::string stop;
            if (!(in >> std::quoted(stop))) return false;
            r.route.push_back(stop);
        }
        history.push_back(r);
    }
    return true;
}
} // namespace storage
#endif
