#include <iostream>
#include <limits>
#include "graph/Graph.h"

int main() {
    Graph city;

    // Starter city map. Road distances are illustrative kilometres.
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

    std::cout << "====================================\n";
    std::cout << "      SMART BIKE TAXI PLATFORM\n";
    std::cout << "====================================\n";
    std::cout << "Available locations:\n";
    city.printLocations();

    std::string source, destination;
    std::cout << "\nEnter pickup location: ";
    std::getline(std::cin, source);
    std::cout << "Enter destination: ";
    std::getline(std::cin, destination);

    const auto result = city.shortestPath(source, destination);
    if (!result.reachable) {
        std::cout << "\nNo route found. Check the location names.\n";
        return 0;
    }

    constexpr double BASE_FARE = 20.0;
    constexpr double FARE_PER_KM = 10.0;
    const double fare = BASE_FARE + result.distance * FARE_PER_KM;

    std::cout << "\nShortest route:\n";
    for (std::size_t i = 0; i < result.path.size(); ++i) {
        if (i) std::cout << " -> ";
        std::cout << result.path[i];
    }
    std::cout << "\nDistance: " << result.distance << " km\n";
    std::cout << "Estimated fare: Rs. " << fare << "\n";
    std::cout << "\nStarter milestone complete: city graph + Dijkstra.\n";
    return 0;
}
