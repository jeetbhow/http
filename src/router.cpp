#include "router.h"

#include <iostream>
#include <memory>
#include <stdexcept>

#include "utils.h"

using namespace std;


// precond: route path is valid
void Router::add(HttpMethod method, const string& path) {
    // E.g. /users/add/:id
    if (!path.starts_with("/")) {
        throw invalid_argument("route must begin with /");
    }

    vector<string> segments = split(path, '/');
    
    RouterNode* curr = &root_;
    for (const string& s : segments) {
        // Segment is a parameter
        if (s.starts_with(":")) {
            if (!curr->_dynamic_child) {
                curr->_dynamic_child = make_unique<RouterNode>(s);
            }
            curr = curr->_dynamic_child.get(); 
            continue;
        }

        // Segment is not a parameter
        auto it = curr->_static_children.find(s);
        if (it == curr->_static_children.end()) {
            curr->_static_children[s] = make_unique<RouterNode>(s);
        }
        curr = curr->_static_children[s].get();
    }

    curr->_routes.insert_or_assign(method, path);
    size_++;
}

// precond: route path is valid
bool Router::contains(HttpMethod method, const string &path) const {
    if (!path.starts_with("/")) {
        throw invalid_argument("route must begin with /");
    }

    vector<string> segments = split(path, '/');

    const RouterNode* curr = &root_;
    for (const string& s : segments) {
        // Check if the segment matches a regular path segment first. 
        auto it = curr->_static_children.find(s);

        // The simplest case is that it does, in which case we keep traversing.
        if (it != curr->_static_children.end()) {
            curr = curr->_static_children.at(s).get();
            continue;
        }

        // Parameters match only nonempty segments.
        if (!s.empty() && curr->_dynamic_child) {
            curr = curr->_dynamic_child.get();
        } else {
            return false;
        }
    }
    
    return curr->_routes.contains(method);
}

void Router::print() const {
    print(root_, "");
}


void Router::print(const RouterNode& node, const string& prefix) const {
    size_t i = 0;
    const size_t child_count = node._static_children.size() + (node._dynamic_child ? 1 : 0);
    const auto print_child = [&](const RouterNode& child) {
        const bool isLast = (++i == child_count);

        cout << prefix
                  << (isLast ? "└── " : "├── ")
                  << child._val;

        if (!child._routes.empty()) {
            cout << " [";
            const char* separator = "";
            for (const auto& [method, path] : child._routes) {
                cout << separator << method_name(method);
                separator = ", ";
            }
            cout << ']';
        }
        cout << '\n';

        print(
            child,
            prefix + (isLast ? "    " : "│   ")
        );
    };

    for (const auto& [seg, child] : node._static_children) {
        print_child(*child);
    }
    if (node._dynamic_child) {
        print_child(*node._dynamic_child);
    }
}
