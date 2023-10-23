#ifndef SERVER_HPP
#define SERVER_HPP

#include <cstddef>
#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "genutil.hpp"

class Server
{
    public:
        int startServer();
        int stopServer();
        ~Server();

    private:
        Server();
        int _sockfd;
        struct sockaddr_in  _sin;
        // Server(Server const & src);
		// Server&	operator=(Server const & rhs);

        const char *_filename;
};

#endif
