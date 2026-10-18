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

private:
    struct RouterNode {
        RouterNode() = default;
        
        std::map<HttpMethod, std::string> _routes;                                          // Registered methods and full paths.
        std::unordered_map<std::string, std::unique_ptr<RouterNode>> _static_children;      // Multiple non-param children.
        std::unique_ptr<RouterNode> _dynamic_child;                                         // Optional param. Multiple params are ambigious.
    };

    friend class RouterTest;

    /**
     * @brief Recursive helper method for `contains` that traverses as deep into the structure as
     * possible and finds the closest `RouterNode` that matches.
     * 
     * @param method The method for the route. 
     * @param path The path for the route.
     * @param curr The current node that we're analyzing in the recursive structure.
     * @return RouterNode* A pointer to the node that we have found.
     */
    RouterNode* find(HttpMethod method, const std::string& path, RouterNode& curr) const;
        
    RouterNode root_;
};
