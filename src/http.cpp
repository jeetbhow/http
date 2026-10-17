#include "http.h"

#include <expected>
#include <sstream>
#include <string>
#include <vector>
#include <iostream>


HttpParseStatus parse_request_line(
    HttpRequest& request, 
    const std::string& request_line
) {
    std::stringstream ss{request_line};
    std::string token;
    std::vector<std::string> tokens;
    while (std::getline(ss, token, ' ')) {
        std::transform(
            token.begin(), 
            token.end(), 
            token.begin(), 
            [](unsigned char c){ return std::tolower(c); }
        );
        tokens.push_back(token);
    }

    if (tokens.size() != 3) {
        return HttpParseStatus::InvalidRequestLine;
    }

    std::string method = tokens[0];
    std::string target = tokens[1];
    std::string version = tokens[2];

    size_t slash = version.find("/");
    if (slash == std::string::npos) {
        std::cerr << "could not find / in version, possible bad formating";
        return HttpParseStatus::InvalidRequestLine;
    }

    version = version.substr(slash + 1);

    if (!HTTP_VERSIONS.contains(version)) {
        std::cerr << std::format("Version {} is not supported\n", version);
        return HttpParseStatus::InvalidVersion;
    }

    if (!HTTP_METHODS.contains(method)) {
        std::cerr << std::format("method {} is not supported", method);
        return HttpParseStatus::InvalidMethod;
    }

    request.method = HTTP_METHODS.at(method);
    request.target = target;
    request.version = version;

    return HttpParseStatus::Ok;
}

std::expected<HttpRequest, HttpParseStatus> HttpRequestParser::parse() {
    HttpRequest request;
    std::vector<std::string> tokens;
    
    size_t end = _start;
    while ((end = _request.find("\r\n", _start)) != std::string::npos) {
        std::string token = _request.substr(_start, end - _start);
        tokens.push_back(token);
        
        _start = end + 2;
    }

    HttpParseStatus status = parse_request_line(request, tokens[0]);
    if (status != HttpParseStatus::Ok) {
        return std::unexpected(status);
    }

    return request;
}