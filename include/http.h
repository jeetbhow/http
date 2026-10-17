#pragma once

#include <unordered_map>
#include <string>
#include <expected>
#include <optional>


enum class HttpMethod {
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
    InvalidRequestLine
};

struct HttpRequest {
    HttpMethod method;
    std::string target;
    std::string version;
    std::unordered_map<std::string, std::string> headers;
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
         * laid out in an `std::string`. Note that this constructor uses move semantics
         * and thus the `std::string` is consumed after use.
         *
         * @param request A `std::string` containing the entire request, including the 
         * CRLF separating the request line, headers, and body. 
         */
        HttpRequestParser(std::string&& request) : _request(std::move(request)) {}

        /**
         * @brief Parse out the contents of the request and return an HttpRequest struct
         * that contains the important fields and methods for processing it.

         * @return HttpRequest An object that contains the parsed request informtion.
         */
        std::expected<HttpRequest, HttpParseStatus> parse();

    private:
        size_t _start = 0;                      // Cursor used for parsing.
        std::string _request;                   // The entire request.
};

