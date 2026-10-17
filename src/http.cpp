#include "http.h"

#include <expected>
#include <algorithm>
#include <cctype>
#include <format>
#include <sstream>
#include <string>
#include <vector>
#include <iostream>

#include "utils.h"

const char* method_name(HttpMethod method) {
    switch (method) {
        case HttpMethod::GET: return "GET";
        case HttpMethod::POST: return "POST";
        case HttpMethod::DELETE: return "DELETE";
        case HttpMethod::PUT: return "PUT";
        case HttpMethod::UNINTIALIZED: return "UNINITIALIZED";
    }
    return "UNKNOWN";
}

std::optional<std::string> HttpRequest::get_header(const std::string& name) const {
    std::string key = name;
    std::transform(
        key.begin(), 
        key.end(), 
        key.begin(), 
        [](unsigned char c) {
            return std::tolower(c);
        }
    );

    if (!_headers.contains(key)) {
        return std::nullopt;
    }
    
    return _headers.at(key);
}


std::expected<HttpRequest, HttpParseStatus> HttpRequestParser::parse() {
    HttpRequest request{};
    const auto request_line_status = parse_request_line(request);
    if (request_line_status != HttpParseStatus::Ok) {
        return std::unexpected(request_line_status);
    }

    const auto headers_status = parse_headers(request);
    if (headers_status != HttpParseStatus::Ok) {
        return std::unexpected(headers_status);
    }

    return request;
}

HttpParseStatus HttpRequestParser::parse_headers(HttpRequest& request) {
    std::unordered_map<std::string, std::string> headers;
    size_t end = _start;
    while ((end = _request.find("\r\n", _start)) != std::string::npos) {
        // Empty line indicates end of headers.
        if (end == _start) {
            _start = end + 2;
            break;
        }
        
        size_t colon = _request.find(":", _start);
        if (colon == std::string::npos || colon >= end) {
            return HttpParseStatus::MalformedHeader;
        }

        std::string key = _request.substr(_start, colon - _start);
        std::string val = trim(
            _request.substr(colon + 1, end - (colon + 1))
        );

        // We store all header names as lowercase.
        std::transform(
            key.begin(), 
            key.end(), 
            key.begin(), 
            [](unsigned char c) {
                return std::tolower(c);
            }
        );

        headers[key] = val;
        _start = end + 2;
    }

    request._headers = std::move(headers);

    return HttpParseStatus::Ok;
}

HttpParseStatus HttpRequestParser::parse_request_line(HttpRequest& request) {
    const size_t end = _request.find("\r\n", _start);
    if (end == std::string::npos) {
        return HttpParseStatus::InvalidRequestLine;
    }

    std::stringstream ss{_request.substr(_start, end - _start)};
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
    request.target = std::move(target);
    request.version = std::move(version);
    _start = end + 2;
    return HttpParseStatus::Ok;
}
