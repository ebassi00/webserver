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

int Server::readRequest(t_connInfo &conn, std::string req)
{
    std::string key;
    std::string comp[] = {"GET", "POST", "DELETE", "PUT", "HEAD"};

    conn.request.line = req.substr(req.find_first_of("/"), req.npos);
    conn.request.line = conn.request.line.substr(0, conn.request.line.find_first_of("\r\n"));
    conn.path = conn.request.line.substr(0, conn.request.line.find_first_of(" "));

    if (conn.path.find("registered.html?") != conn.path.npos)
    {
        conn.request.arguments = conn.path.substr(conn.path.find("?") + 1);
        conn.path = conn.path.substr(0, conn.path.find("?"));
    }

    if (req.find_first_of(" \t") != std::string::npos)
        conn.request.method = req.substr(0, req.find_first_of(" \t"));
    if (req.find_first_of("/") != std::string::npos)
    {
        conn.request.path = req.substr(req.find_first_of("/"));
        conn.request.path = conn.request.path.substr(0, conn.request.path.find_first_of(" "));
    }

    int i;
    for (i = 0; i < 5; i++)
    {
        if (!conn.request.method.compare(comp[i]))
            break ;
    }
    if (i == 5)
        return (0);
    req = req.substr(req.find_first_of("\n") + 1);
    while (req.find_first_of("\n") != std::string::npos)
    {
        key = req.substr(0, req.find_first_of(":"));
        if (conn.request.headers.find(key) != conn.request.headers.end())
            conn.request.headers[key] = req.substr(req.find_first_of(":") + 2, req.find_first_of("\n") - req.find_first_of(":") - 2);
        req = req.substr(req.find_first_of("\n") + 1);
    }
    if (conn.request.headers.find(key) == conn.request.headers.end())
        return (1);
    conn.request.headers[key] = req.substr(req.find_first_of(":") + 2, req.find_first_of("\n") - req.find_first_of(":") - 2);
    return (1);
}

 /*
    Funzione di indirizzamento all'utente di pagine di errore custom / default
 */

void Server::defaultAnswerError(int error, t_connInfo conn)
{
    std::string tmpErr;
    std::stringstream body;
    std::ifstream file;

    if (conn.config.errorPages.size() && error != 500)
    {
        body << error;
        for (std::vector<std::string>::iterator it = conn.config.errorPages.begin(); it != conn.config.errorPages.end(); it++)
        {
            if (body.str() == (*it).substr(0, 3))
            {
                if (*(_config->root.end() - 1) != '/')
					_config->root.push_back('/');
				file.open((_config->root + *it).c_str());
				if (!file)
				{
					file.close();
					defaultAnswerError(500, conn);
					return ;
				}
				break ;
            }
        }
    }

    switch (error)
    {
        case 100: tmpErr = "100 Continue"; break ;
		case 200: tmpErr = "200 OK"; break ;
		case 201: tmpErr = "201 Created"; break ;
		case 202: tmpErr = "202 Accepted"; break ;
		case 203: tmpErr = "203 Non-Authoritative Information"; break ;
		case 204: tmpErr = "204 No content"; break ;
		case 205: tmpErr = "205 Reset Content"; break ;
		case 206: tmpErr = "206 Partial Content"; break ;
		case 400: tmpErr = "400 Bad Request"; break ;
		case 401: tmpErr = "401 Unauthorized"; break ;
		case 402: tmpErr = "402 Payment Required"; break ;
		case 403: tmpErr = "403 Forbidden"; break ;
		case 404: tmpErr = "404 Not Found"; break ;
		case 405: tmpErr = "405 Method Not Allowed"; break ;
		case 406: tmpErr = "406 Not Acceptable"; break ;
		case 411: tmpErr = "411 Length Required"; break ;
		case 413: tmpErr = "413 Request Entity Too Large"; break ;
		case 500: tmpErr = "500 Internal Server Error"; break ;
		case 501: tmpErr = "501 Not Implemented"; break ;
		case 510: tmpErr = "510 Not Extended"; break ;
		default: break ;
    }

    conn.response.line = "HTTP/1.1 " + tmpErr;

    if (file.is_open())
    {
        body.clear();
        body << file.rdbuf();
        file.close();
        conn.response.body = body.str();
        conn.response.body.erase(0, 3);
        body.str("");
        body << conn.response.body.length();
        findKey(conn.response.headers, "Content-Length")->second = body.str();
		tmpErr.clear();
    }
    else if (error != 100)
    {
        conn.response.body = "<html>\n<head><title>" + tmpErr + "</title></head>\n<body>\n<center><h1>" + tmpErr + "</h1></center>\n<hr><center>webserv</center>\n</body>\n</html>\n";
		body.str("");
		body << conn.response.body.length();
        std::cout << body.str() << std::endl;
		findKey(conn.response.headers, "Content-Length")->second = body.str();
		tmpErr.clear();
    }

    if (error == 405)
	{
		for (std::vector<std::string>::iterator iter = conn.config.allowedMethods.begin(); iter != conn.config.allowedMethods.end(); iter++) {
			if (findKey(conn.response.headers, "Allow")->second.length())
				findKey(conn.response.headers, "Allow")->second += " " + *iter;
			else
				findKey(conn.response.headers, "Allow")->second += *iter;
		}
	}

    tmpErr = conn.response.line + "\r\n";
    std::map<std::string, std::string>::iterator iter = conn.response.headers.begin();
	findKey(conn.response.headers, "Connection")->second = "close";

    while (iter != conn.response.headers.end())
	{
		if ((*iter).second.length())
			tmpErr += (*iter).first + ": " + (*iter).second + "\r\n";
		iter++;
	}
	if (error != 100 && conn.request.method != "HEAD")
		tmpErr += "\r\n" + conn.response.body + "\r\n";
	else
		tmpErr += "\r\n";
	send(conn.fd, tmpErr.c_str(), tmpErr.size(), 0);
}

bool Server::isRegex(std::string path, t_config *config)
{
    for (std::vector<t_location>::iterator it = config->locationRules.begin(); it != config->locationRules.end(); it++)
    {
        if (it->regex && !std::strncmp(it->location.c_str(), &(path.c_str())[path.length() - it->location.length()], it->location.length()))
			return (true);
	}
	return (false);
}

t_config *Server::findConfigByConnection(t_connInfo &conn)
{
    std::string serverName = conn.request.headers["Host"];

    if (serverName.find(":") != serverName.npos)
		serverName = serverName.substr(0, serverName.find(":"));
    
    if (!std::strncmp(serverName.c_str(), "localhost", 9) || serverName.find("localhost"))
        return (&(*(_configs.begin())));
    
    for (std::vector<t_config>::iterator iter = _configs.begin(); iter != _configs.end(); iter++)
	{
		for (std::vector<std::string>::iterator serverNameIt = iter->server_name.begin(); serverNameIt != iter->server_name.end(); serverNameIt++) 
        {
			if (!serverName.compare(*serverNameIt))
				return (&(*iter));
		}
	}
	return (&(*(_configs.begin())));
}

t_location *Server::findLocationBlock(t_connInfo &conn)
{
    bool regex;
    t_location *ret = NULL;

    _config = findConfigByConnection(conn);
    regex = isRegex(conn.request.path, _config);

    for (std::vector<t_location>::iterator it = _config->locationRules.begin(); !regex && it != _config->locationRules.end(); it++)
    {
        if (!std::strncmp(conn.request.path.c_str(), (*it).location.c_str(), (*it).location.length()) && !(*it).regex)
        {
            if (!ret || (*it).location.length() > ret->location.length())
				ret = &(*it);
        }
    }

    if (!ret && !regex)
    {
        for (std::vector<t_location>::iterator it = _config->locationRules.begin(); !regex && it != _config->locationRules.end(); it++)
        {
            if (!std::strncmp(conn.request.path.c_str(), (*it).location.c_str(), conn.request.path.length()) && !(*it).regex)
            {
                if (!ret || (*it).location.length() > ret->location.length())
                    ret = &(*it);
            }
        }
    }
    
    if (regex)
    {
        for (std::vector<t_location>::iterator it = _config->locationRules.begin(); !regex && it != _config->locationRules.end(); it++)
        {
            if (!std::strncmp((*it).location.c_str(), &(conn.request.path.c_str())[conn.request.path.length() - (*it).location.length()], (*it).location.length()) && !(*it).regex)
            {
                if (!ret || !ret->regex || (*it).location.length() > ret->location.length())
                    ret = &(*it);
            }
        }
    }

    if (!ret)
		defaultAnswerError(404, conn);
    return (ret);
}

t_config Server::findConfigBlock(t_connInfo &conn)
{
    t_config ret = *(findConfigByConnection(conn));
    std::string toCompare[9] = {"root", "autoindex", "index", "error_page", "client_max_body_size", "allowed_methods", "try_files", "return", "cgi_pass"};
    std::cout << conn.location->location << std::endl;
    return ret;
}

int Server::handleClient(int fd)
{
    std::vector<t_connInfo>::iterator it = _connections.begin();
    std::string tmp;
    while (it != _connections.end())
    {
        if (it->fd == fd)
            break ;
        it++;
    }
    if (it == _connections.end())
        return (0);
    
    // struct linger l;

    // l.l_onoff = 1;
    // l.l_linger = 0;
    unsigned char buff;

    if (it->request.method == "")
    {
        recv(fd, &buff, sizeof(unsigned char), 0);
        tmp += buff;
        it->buffer.push_back(buff);
        if (it->buffer.find("\r\n\r\n") == it->buffer.npos) return 0;

        if (!readRequest(*it, it->buffer))
        {
            defaultAnswerError(400, *it);
            _connections.erase(it);
            return (1);
        }
        it->location = findLocationBlock(*it);
        if (!it->location)
		{
			defaultAnswerError(404, *it);
			return (1);
		}
        it->config = findConfigBlock(*it);
    }
    return 1;
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