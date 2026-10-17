#pragma once

#include <vector>
#include <iostream>
#include <string>

const std::string WHITESPACE = " \t\n\r\f\v";

template <typename T>
void print_vector(std::vector<T> vec) {
    std::cout << "[";
    for (size_t i = 0; i < vec.size(); i++) {
        std::cout << vec[i];
        if (i < vec.size() - 1) {
            std::cout << ", ";
        }
    }
    std::cout << "]\n";
}

std::vector<std::string> split(const std::string& s, char delim); 
std::string ltrim(const std::string& s);
std::string rtrim(const std::string& s);
std::string trim(const std::string& s);
