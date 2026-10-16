#pragma once

class HttpServer {
    public:
        /**
         * @brief Construct a new HttpServer object
         */
        HttpServer() = default;

        /**
         * @brief Create a server socket and bind it to a port. Mark the server socket 
         * as passive, meaning it can accept incoming connections. 
         * 
         * @param port The port that you want the server to listen on.
         * @return std::expected<bool, std::error_code> Whether or not the operation 
         */
        void listen(unsigned short port);
        
    
    private:
        /**
         * @brief Initialize the hints struct so that `getaddrinfo` will find the addresses that 
         * we are interested in. The following fields are set: 
         * - ai_family -> AF_UNSPEC
         * - ai_socktype -> SOCK_STREAM
         * - ai_flags -> AI_PASSIVE 
         * 
         *
         * @param hints The addrinfo hints struct to initialize.
         */
        void _init_hints(struct addrinfo& hints);

        /**
         * @brief Process an incoming connection. If the request is not a valid HTTP request
         * then reject it and continue to listen for requests.
         * 
         * @param client_socket The file descriptor for the incoming connection.
         */
        void _process_client(int client_socket);

        const static unsigned short BACKLOG = 10;
        int _server_socket = -1;
};