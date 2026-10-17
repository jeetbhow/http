#include "http.h"
#include "server.h"

#include <iostream>
#include <exception>
#include <cstdlib>
#include <cstring>
#include <cerrno>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>

int main() {
    std::string example_request = 
    "GET /index.html HTTP/1.1\r\n"
    "Host: example.com\r\n"
    "User-Agent: Mozilla/5.0\r\n"
    "Accept: text/html\r\n"
    "Connection: keep-alive\r\n";

    HttpRequestParser parser{std::move(example_request)};
    parser.parse();
}
