#pragma once

#include <memory>
#include <string>
#include <vector>

struct Node {
    explicit Node(std::string val) : value(std::move(val)) {}
    std::string value;                                       // Each value represents a prefix component in the final route.
    std::vector<std::unique_ptr<Node>> children;             // Each child extends the prefix.
    bool is_route = false;                                   // Determines if the current node is a route.
};

class Router {
    public:
        Router() = default;

        void add(const std::string& route);
        bool contains(const std::string& route) const;
        void remove(const std::string& route);
        void print() const;

    private:
        friend class RouterAddTest;

        void print(const Node& node, const std::string& prefix) const;
        
        Node _root = Node{ "" };
        size_t _size = 0;
};
