#pragma once

#include "router.h"

#include <memory>
#include <map>
#include <string>

#include "http.h"


class Router {
public:
    Router() = default;

    void add(HttpMethod method, const std::string& path);
    bool contains(HttpMethod method, const std::string& path) const;
    void remove(const std::string& route);
    void print() const;

private:
    struct RouterNode {
        RouterNode() = default;
        RouterNode(std::string val) : _val(std::move(val)) {}
         
        std::string _val;                                                                   // Contains a path segment.
        std::map<HttpMethod, std::string> _routes;                                          // Registered methods and full paths.
        std::unordered_map<std::string, std::unique_ptr<RouterNode>> _static_children;      // Multiple non-param children.
        std::unique_ptr<RouterNode> _dynamic_child;                                         // Optional param. Multiple params are ambigious.
    };

    friend class RouterTest;

    void print(const RouterNode& node, const std::string& prefix) const;
        
    RouterNode root_ = RouterNode{ "" };
    size_t size_ = 0;
};
