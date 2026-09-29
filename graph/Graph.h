#ifndef SMART_BIKE_TAXI_GRAPH_H
#define SMART_BIKE_TAXI_GRAPH_H

#include <algorithm>
#include <functional>
#include <iostream>
#include <limits>
#include <cmath>
#include <queue>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

class Graph {
public:
    struct PathResult {
        bool reachable = false;
        double distance = 0.0;
        std::vector<std::string> path;
    };

    bool addLocation(const std::string& name) {
        if (name.empty() || index_.count(name)) return false;
        const int id = static_cast<int>(names_.size());
        index_[name] = id;
        names_.push_back(name);
        adjacency_.emplace_back();
        return true;
    }

    bool addRoad(const std::string& from, const std::string& to, double km) {
        if (!std::isfinite(km) || km < 0.0 || !index_.count(from) ||
            !index_.count(to)) return false;
        const int u = index_.at(from), v = index_.at(to);
        adjacency_[u].push_back({v, km});
        adjacency_[v].push_back({u, km}); // Roads are two-way in this starter map.
        return true;
    }

    void printLocations() const {
        for (const auto& name : names_) std::cout << " - " << name << '\n';
    }

    PathResult shortestPath(const std::string& source,
                            const std::string& destination) const {
        PathResult result;
        if (!index_.count(source) || !index_.count(destination)) return result;

        const int start = index_.at(source), finish = index_.at(destination);
        const double INF = std::numeric_limits<double>::infinity();
        std::vector<double> dist(names_.size(), INF);
        std::vector<int> parent(names_.size(), -1);

        using State = std::pair<double, int>;
        std::priority_queue<State, std::vector<State>, std::greater<State>> pq;
        dist[start] = 0.0;
        pq.push({0.0, start});

        while (!pq.empty()) {
            const auto [currentDistance, u] = pq.top();
            pq.pop();
            if (currentDistance > dist[u]) continue;
            if (u == finish) break;

            for (const auto& [v, weight] : adjacency_[u]) {
                const double candidate = currentDistance + weight;
                if (candidate < dist[v]) {
                    dist[v] = candidate;
                    parent[v] = u;
                    pq.push({candidate, v});
                }
            }
        }

        if (dist[finish] == INF) return result;

        result.reachable = true;
        result.distance = dist[finish];
        for (int at = finish; at != -1; at = parent[at]) {
            result.path.push_back(names_[at]);
        }
        std::reverse(result.path.begin(), result.path.end());
        return result;
    }

private:
    std::vector<std::string> names_;
    std::unordered_map<std::string, int> index_;
    std::vector<std::vector<std::pair<int, double>>> adjacency_;
};

#endif
