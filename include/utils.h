#pragma once

#include <vector>
#include <iostream>

template <typename T>
void print_vector(std::vector<T> vec) {
    std::cout << "[ ";
    for (const T& elem : vec) {
        std::cout << elem << " ";
    }
    std::cout << "]";
}