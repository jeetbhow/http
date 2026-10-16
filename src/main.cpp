#include <asm-generic/socket.h>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <sys/socket.h>
#include <netdb.h>
#include <cerrno>
#include <unistd.h>

/**
 * Note: Since I'm not too familiar with C's socket API I'm using the comments here as my own reference.
 * I will likely delete much of the information here some point later when this project is more developed.
 */

int main() {
    // Hints help getaddrinfo fill in the addrinfo struct
    struct addrinfo hints;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;                    // IPv4 vs IPv6 doesn't matter.
    hints.ai_socktype = SOCK_STREAM;                // TCP connection
    hints.ai_flags = AI_PASSIVE;                    // This is going to be a server, not a client
    
    // The head of the linked list returned by getaddrinfo
    struct addrinfo *res;

    // Creates a linked list of addresses for the server, and points res to its head.
    int status = getaddrinfo(
        nullptr, 
        "8080",
        &hints,
        &res 
    );

    // getaddrinfo returns 0 on success and a negative number on failure
    if (status != 0) {
        std::cerr << gai_strerror(status) << "\n";
        return EXIT_FAILURE;
    }

    // Try find an address to bind to. Note that getaddrinfo returns
    // a pointer to the head of a linked list, and each node contains an address.
    addrinfo *p = res;
    int server_sock = -1;
    do {
        server_sock = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (server_sock < 0) {
            continue;
        }

        // The setsockopt function is used to enable the SO_REUSEADDR option. This option
        // allows you to immediately reuse the port after closing your server and is helpful
        // for testing.
        int yes = 1;
        if (setsockopt(
            server_sock, 
            SOL_SOCKET, 
            SO_REUSEADDR, 
            &yes, 
            sizeof yes
        ) != 0) {
            close(server_sock);
            server_sock = -1;
            continue;
        }
    
        // Once we've found an address we can bind to the socket.
        if (bind(server_sock, p->ai_addr, p->ai_addrlen) == 0) {
            break;
        }

        close(server_sock);
        server_sock = -1;
    } while ((p = p->ai_next));

    // Once we've bound to an address we don't need the addrinfo struct anymore.
    freeaddrinfo(res);

    // If we didn't bind to anything at all then that means there's nothing we can really do.
    // Quit the program at this point.
    if (server_sock == -1) {
        std::cerr << "failed to bind to any address\n";
        return EXIT_FAILURE;
    }

    // Conversly, if we did, then make the socket passive. A passive socket is capable of accepting 
    // requests.
    if (listen(server_sock, 10) != 0) {
        std::cerr << "listen failed: " << strerror(errno) << "\n";
        close(server_sock);
        return EXIT_FAILURE;
    }

    std::cout << "Listening on port 8080\n";

    struct sockaddr_storage client_addr; 
    socklen_t client_addrlen = sizeof(client_addr);
    
    // Accept the incoming connection.
    int client_sock = accept(
        server_sock, 
        reinterpret_cast<sockaddr*>(&client_addr), 
        &client_addrlen
    );

    if (client_sock < 0) {
        std::cerr << "accept failed: " << strerror(errno) << "\n";
        close(client_sock);
        close(server_sock);
        return EXIT_FAILURE;
    }

    // Read data being sent over the connection.
    char buf[4096];
    ssize_t bytes_recv = 0;
    while ((bytes_recv = recv(client_sock, buf, sizeof(buf), 0)) > 0) {
        std::cout.write(buf, bytes_recv);
    }

    if (bytes_recv < 0) {
        std::cerr << "recv failed: " << strerror(errno) << "\n";
        return EXIT_FAILURE;
    }

    std::string response = "HTTP/1.1 200 OK\r\n\r\n";
    send(client_sock, response.data(), response.length(), 0);

    close(client_sock);
    close(server_sock);
    return EXIT_SUCCESS;
}
