#include "server.h"

#include <iostream>
#include <cstdlib>
#include <cstring>
#include <stdexcept>
#include <string>
#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>

void HttpServer::init_hints(struct addrinfo& hints) {
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;        // IPv4 or IPv6 is fine
    hints.ai_socktype = SOCK_STREAM;    // TCP
    hints.ai_flags = AI_PASSIVE;        // Server socket
}

void HttpServer::listen(unsigned short port) {
    // Get a network address to bind to. 
    struct addrinfo hints;
    struct addrinfo *addrs;
    int status;

    init_hints(hints);
    status = getaddrinfo(
        nullptr, 
        std::to_string(port).data(), 
        &hints, 
        &addrs
    );

    if (status != 0) {
        std::string err = gai_strerror(status);
        throw std::runtime_error("failed to create server: " + err + "\n");
    }

    do {
        _server_socket = socket(
            addrs->ai_family, 
            addrs->ai_socktype, 
            addrs->ai_protocol
        );

        if (_server_socket < 0) {
            continue;
        }

        int yes = 1;
        status = setsockopt(
            _server_socket,
            SOL_SOCKET,
            SO_REUSEADDR,
            &yes,
            sizeof yes
        );

        if (status < 0) {
            continue;
            close(_server_socket);
            _server_socket = -1;
        }

        // Bind the socket to the address.
        if (bind(_server_socket, addrs->ai_addr, addrs->ai_addrlen) == 0) {
            break;
        }

        close(_server_socket);
        _server_socket = -1;
    } while ((addrs = addrs->ai_next));

    freeaddrinfo(addrs);

    // Note: If the socket failed to be created and bound to an address, then the file descriptor
    // will still be -1 at the end of this loop. In that case, we throw a runtime error as the server
    // cannot be created.
    if (_server_socket == -1) {
        std::string err = strerror(errno);
        throw std::runtime_error("failed to create server: " + err + "\n");
    }

    status = ::listen(_server_socket, BACKLOG);
    if (status < 0) {
        std::string err = strerror(errno);
        throw std::runtime_error("failed to create server: " + err + "\n");
    }

    std::cout << std::format("Now listening on port {}.\n" , port);
}