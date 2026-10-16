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
    HttpServer server;
    
    try {
        server.listen(8080);
    } catch (std::exception& e) {
        std::cout << e.what() << "\n";
    }
}
