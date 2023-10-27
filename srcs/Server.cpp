#include "../includes/Server.hpp"

// Server::Server(Server const & src)
// {}

// Server::Server&	operator=(Server const & rhs)
// {}

Server::Server(t_config config)
{
    _config = NULL;
    _configs.push_back(config);
    memset(&_sin, '\0', sizeof(_sin));
    this->_sin.sin_family = AF_INET;
    this->_sin.sin_addr.s_addr = inet_addr((*(_configs.begin())).host.c_str());
    this->_sin.sin_port = htons((*(_configs.begin())).port);

    if(!this->startServer())
        die("Failed to start the Server.");
}

int Server::acceptConnection(int sockfd)
{
    socklen_t addrlen;
	struct sockaddr_storage remoteaddr;
	int		tmpfd;
	addrlen = sizeof remoteaddr;

	tmpfd = accept(sockfd, (struct sockaddr *)&remoteaddr, &addrlen);
    if (tmpfd == -1)
        die("Failed to accept incoming connection.");
    
    _connections.push_back(t_connInfo(tmpfd));
    return (tmpfd);
}

int Server::handleClient(int fd)
{
    std::vector<t_connInfo>::iterator it = _connections.begin();
    while (it != _connections.end())
    {
        if (it->fd == fd)
            break ;
        it++;
    }
    if (it == _connections.end())
        return (0);
    
    struct linger l;

    l.l_onoff = 1;
    l.l_linger = 0;
    unsigned char buff;

    if (it->request.method == "")
    {
        recv(fd, &buff, sizeof(unsigned char), 0);
        it->buffer.push_back(buff);
        if (it->buffer.find("\r\n\r\n") == it->buffer.npos) return 0;
    }
}

int Server::startServer()
{
    int i = 1;
    _sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (_sockfd == -1)
        return (int_error("Socket error!"));
    if (setsockopt(_sockfd, SOL_SOCKET, SO_REUSEADDR, (const char *)&i, sizeof(i)) < 0) // Per evitare che al riavvio del server il socket sia ancora in uso
        return (int_error("setsockopt error!"));
    if (bind(_sockfd, (struct sockaddr *)&_sin, sizeof(_sin)) < 0)
        return (int_error("Bind error!"));
    if (listen(_sockfd, 200) == -1)
        return(int_error("Listen error!"));

    return (1);
}

int Server::stopServer()
{
    if (close(_sockfd))
		return (die("Error closing _sockfd"));
	return 1;
}

int Server::getSocket() { return _sockfd; }

int Server::getConfigSize() { return (_configs.size()); }

Server::~Server()
{
}