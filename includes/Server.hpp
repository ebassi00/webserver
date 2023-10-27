#ifndef SERVER_HPP
#define SERVER_HPP

#include "WebServ.hpp"
#include "genutil.hpp"

class Server
{
    public:
        Server(t_config config);
        int startServer();
        int stopServer();
        ~Server();
        t_config*   getConfig();
        int getConfigSize();
        int getSocket();
        int acceptConnection(int sockfd);
        int handleClient(int fd);

    private:
        Server();
        int _sockfd;
        struct sockaddr_in  _sin;
        std::vector<t_connInfo>	_connections;
        // Server(Server const & src);
		// Server&	operator=(Server const & rhs);

        const char *_filename;
        std::vector<t_config>   _configs;
		t_config*				_config;
};

#endif
