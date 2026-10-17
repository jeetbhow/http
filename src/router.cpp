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
        if (!curr->_children.contains(s)) {
            curr->_children[s] = std::make_unique<RouterNode>(s);
        }
        curr = curr->_children[s].get();
    }

    curr->_route = Route { method, path };
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
                  << child->_val
                  << '\n';

        print(
            *child,
            prefix + (isLast ? "    " : "│   ")
        );
    }   
}
