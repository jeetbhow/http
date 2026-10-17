#include "router.h"

#include <iostream>
#include <memory>

#include "utils.h"

// precond: route path is valid
void Router::add(HttpMethod method, const std::string& path) {
    // E.g. /users/add/:id
    if (!path.starts_with("/")) {
        throw std::invalid_argument("route must begin with /");
    }

    std::vector<std::string> segments = split(path, '/');
    
    RouterNode* curr = &_root;
    for (const std::string& s : segments) {
        auto it = curr->_children.find(s);

        if (it == curr->_children.end()) {
            curr->_children[s] = std::make_unique<RouterNode>(s);
        }

        curr = curr->_children[s].get();
    }

    curr->_routes.insert_or_assign(method, path);
}

// precond: route path is valid
bool Router::contains(HttpMethod method, const std::string &path) const {
    if (!path.starts_with("/")) {
        throw std::invalid_argument("route must begin with /");
    }

    std::vector<std::string> segments = split(path, '/');

    const RouterNode* curr = &_root;
    for (const std::string& s : segments) {
        auto it = curr->_children.find(s);

        if (it == curr->_children.end()) {
            return false;
        }

        curr = it->second.get();
    }
    
    return curr->_routes.contains(method);
}

void Router::print() const {
    print(_root, "");
}


void Router::print(const RouterNode& node, const std::string& prefix) const {
    size_t i = 0;
    for (const auto& [seg, child] : node._children) {

        const bool isLast = (++i == node._children.size());

        std::cout << prefix
                  << (isLast ? "└── " : "├── ")
                  << child->_val;

        if (!child->_routes.empty()) {
            std::cout << " [";
            const char* separator = "";
            for (const auto& [method, path] : child->_routes) {
                std::cout << separator << method_name(method);
                separator = ", ";
            }
            std::cout << ']';
        }
        std::cout << '\n';

        print(
            *child,
            prefix + (isLast ? "    " : "│   ")
        );
    }   
}
