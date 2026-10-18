#include <iostream>
#include <cstdlib>
#include <cstring>
#include <cerrno>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>

#include "http.h"
#include "router.h"

int main() {
    Router r;
    r.add(HttpMethod::GET, "/users/:id");
    r.add(HttpMethod::PUT, "/users/:id");
}
