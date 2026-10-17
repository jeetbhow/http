#pragma once

#include <vector>
#include <iostream>
#include <string>

const std::string WHITESPACE = " \t\n\r\f\v";

template <typename T>
void print_vector(std::vector<T> vec) {
    std::cout << "[ ";
    for (const T& elem : vec) {
        std::cout << elem << " ";
    }
    std::cout << "]";
}

std::string ltrim(const std::string& s);
std::string rtrim(const std::string& s);
std::string trim(const std::string& s);
