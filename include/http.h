#pragma once

#include <unordered_map>
#include <string>
#include <expected>
#include <optional>
#include <utility>


enum class HttpMethod {
    UNINTIALIZED,
    GET,
    POST,
    DELETE,
    PUT,
};

enum class HttpVersion {
    ZERO_NINE,
    ONE,
    ONE_ONE,
    TWO,
    THREE
};

enum class HttpParseStatus {
    Ok,
    InvalidMethod,
    InvalidVersion,
    InvalidRequestLine,
    MalformedHeader
};

class HttpRequest {
public:
    HttpMethod method = HttpMethod::UNINTIALIZED;
    std::string target;
    std::string version;

    HttpRequest() = default;
    HttpRequest(HttpMethod method, std::string target, std::string version)
        : method(method), target(std::move(target)), version(std::move(version)) {}

    // All headers are stored lowercase (e.g. Content-Type -> content-type).
    std::optional<std::string> get_header(const std::string& name) const;

private:
    friend class HttpRequestParser;                             // So the parser can create an HTTPRequest.
    std::unordered_map<std::string, std::string> _headers;      
};


static const std::unordered_map<std::string, HttpMethod> HTTP_METHODS = {
    {"get", HttpMethod::GET}
};

static const std::unordered_map<std::string, HttpVersion> HTTP_VERSIONS = {
    {"0.9", HttpVersion::ZERO_NINE},
    {"1.0", HttpVersion::ONE},
    {"1.1", HttpVersion::ONE_ONE},
    {"2.0", HttpVersion::TWO},
    {"3.0", HttpVersion::THREE}
};

class HttpRequestParser {
    public:
        /**
         * @brief Construct a new HttpRequestParser that parses the contents that are
         * laid out in an `std::string`. 
         *
         * @param request A `std::string` containing the entire request, including the 
         * CRLF separating the request line, headers, and body. 
         */
        HttpRequestParser(std::string request) : _request(std::move(request)) {}

        /**
         * @brief Parse out the contents of the request and return an HttpRequest object
         * that contains the important fields and methods for processing it.

         * @return HttpRequest An object that contains the parsed request informtion.
         */
        std::expected<HttpRequest, HttpParseStatus> parse();

    private:
        HttpParseStatus parse_request_line(HttpRequest& request);
        HttpParseStatus parse_headers(HttpRequest& request);

        size_t _start = 0;                      // Cursor used for parsing.
        std::string _request;                   // The entire request.
};

