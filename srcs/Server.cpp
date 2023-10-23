#include "../includes/Server.hpp"

// Server::Server(Server const & src)
// {}

// Server::Server&	operator=(Server const & rhs)
// {}

Server::Server()
{
    // Definizione della struttura sockaddr per il socket
    const char *host = "127.0.0.1";
    int port = 8080;
    memset(&_sin, '\0', sizeof(_sin));
    this->_sin.sin_family = AF_INET;
    this->_sin.sin_addr.s_addr = inet_addr(host);
    this->_sin.sin_port = htons(port);
    this->startServer();
}

int Server::startServer()
{
    int i = 1;
    _sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (_sockfd == -1)
        return (die("Socket error!"));
    if (setsockopt(_sockfd, SOL_SOCKET, SO_REUSEADDR, (const char *)&i, sizeof(i)) < 0) // Per evitare che al riavvio del server il socket sia ancora in uso
        return (die("setsockopt error!"));
    if (bind(_sockfd, (struct sockaddr *)&_sin, sizeof(_sin)) < 0)
        return (die("Bind error!"));
    if (listen(_sockfd, 200) == -1)
        return(die("Listen error!"));

    return (1);
}

int Server::stopServer()
{
    if (close(_sockfd))
		return (die("Error closing _sockfd"));
	return 1;
}

Server::~Server()
{
}