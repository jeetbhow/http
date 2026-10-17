#include <iostream>
#include <cstdlib>
#include <cstring>
#include <cerrno>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>

#include "router.h"
#include "utils.h"

int main() {
    std::string create_id = "/user/create/:id";
    std::string get_id = "/user/:id";

    Router r;
    r.add(create_id);
    r.add(get_id);
    r.print();
}
