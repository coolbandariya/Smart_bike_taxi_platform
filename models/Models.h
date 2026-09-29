#ifndef SMART_BIKE_TAXI_MODELS_H
#define SMART_BIKE_TAXI_MODELS_H

#include <string>
#include <vector>

struct Rider {
    std::string id;
    std::string name;
    std::string phone;
};

struct Driver {
    std::string id;
    std::string name;
    std::string phone;
    std::string location;
    bool available = true;
};

struct RideRequest {
    int requestId = 0;
    std::string riderId;
    std::string pickup;
    std::string destination;
};

struct Ride {
    int rideId = 0;
    std::string riderId;
    std::string driverId;
    std::string pickup;
    std::string destination;
    double distanceKm = 0.0;
    double fare = 0.0;
    std::vector<std::string> route;
    std::string status;
};

#endif
