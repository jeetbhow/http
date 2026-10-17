#pragma once

#include "router.h"

#include <memory>
#include <map>
#include <string>

#include "http.h"


struct Route {
    HttpMethod method; 
    std::string path;
};

class Router {
public:
    Router() = default;

    void add(HttpMethod method, const std::string& route);
    bool contains(const std::string& route) const;
    void remove(const std::string& route);
    void print() const;

private:
    struct RouterNode {
        RouterNode() = default;
        RouterNode(std::string val) : _val(std::move(val)) {}
         
        std::string _val;                                                          // Contains a path segment.
        std::map<HttpMethod, Route> _routes;                                       // Registered methods for this path segment.
        std::unordered_map<std::string, std::unique_ptr<RouterNode>> _children;
    };

    friend class RouterAddTest;

    void print(const RouterNode& node, const std::string& prefix) const;
        
    RouterNode _root = RouterNode{ "" };
    size_t _size = 0;
};
