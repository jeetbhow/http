#include "router.h"

#include <iostream>
#include <memory>

#include "utils.h"

// precond: route is valid
void Router::add(const std::string& route) {
    // /users/add/:id
    if (!route.starts_with("/")) {
        throw std::invalid_argument("route must start with /");
    }

    Node* root = &_root; 
    std::vector<std::string> path_segments = split(route, '/');
    for (const std::string& seg : path_segments) {

        Node* next = nullptr;
        for (auto& child : root->children) {
            if (child->value == seg) {
                next = child.get();
                break;
            }
        }

        if (next == nullptr) {
            auto new_node = std::make_unique<Node>(seg);
            next = new_node.get();    
            root->children.push_back(std::move(new_node));
        }

        root = next;
    }
    root->is_route = true;
}

void Router::print() const {
    print(_root, "");
}


void Router::print(const Node& node, const std::string& prefix) const {
    for (size_t i = 0; i < node.children.size(); ++i) {
        const auto& child = node.children[i];

        bool isLast = (i == node.children.size() - 1);

        std::cout << prefix
                  << (isLast ? "└── " : "├── ")
                  << child->value
                  << '\n';

        print(
            *child,
            prefix + (isLast ? "    " : "│   ")
        );
    }   
}
