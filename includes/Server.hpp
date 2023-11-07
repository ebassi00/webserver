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
        int readRequest(t_connInfo &conn, std::string req);
        t_config *findConfigByConnection(t_connInfo &conn);

    private:
        Server();
        int _sockfd;
        struct sockaddr_in  _sin;
        std::vector<t_connInfo>	_connections;
        void defaultAnswerError(int error, t_connInfo conn);
        t_location *findLocationBlock(t_connInfo &conn);
        bool isRegex(std::string path, t_config *config);
        t_config findConfigBlock(t_connInfo &conn);
        // Server(Server const & src);
		// Server&	operator=(Server const & rhs);

        const char *_filename;
        std::vector<t_config>   _configs;
		t_config*				_config;
};

#endif
