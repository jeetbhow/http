#include "utils.h"

#include <sstream>

std::vector<std::string> split(const std::string& s, char delim) {
    std::stringstream ss{ s };
    std::vector<std::string> result;
    std::string part;

    while (std::getline(ss, part, delim)) {
        result.push_back(part);
    }
    
    return result;
}

std::string ltrim(const std::string& s) {
    size_t start = s.find_first_not_of(WHITESPACE);
    return start == std::string::npos ? "" : s.substr(start);
}

std::string rtrim(const std::string& s) {
    size_t end = s.find_last_not_of(WHITESPACE);
    return end == std::string::npos ? "" : s.substr(0, end + 1);
}

std::string trim(const std::string& s) {
    return ltrim(rtrim(s));
}
