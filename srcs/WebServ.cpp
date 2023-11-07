#include "../includes/WebServ.hpp"

// Server* checkPort(std::vector<Server> &servers, t_config conf)
// {
//     int i;

//     for (std::vector<Server>::iterator iter = servers.begin(); iter != servers.end(); iter++)
// 	{
// 		i = 0;
// 		while (i < iter->getConfigSize())
// 		{
// 			if (iter->getConfig(i++)->port == config.port)
// 				return (&(*iter));
// 		}
// 	}
// 	return (NULL);
// }

WebServ::WebServ(char const *filename)  : _filename(filename == NULL ? "default.conf" : filename)
{
    Server* serv;
    _config.reserve(30);
    _servers.reserve(30);
    Parser(_filename, _config);
    (void)serv;

    std::vector<t_config>::iterator   iterConfig = _config.begin();

    // while (iterConfig != _config.end())
    // {
    //     serv = checkPort(_servers, *iterConfig);
	// 	if (serv)
	// 		serv->addConfig(*iterConfig);
	// 	else
	// 		_servers.push_back(Server(*iterConfig));
	// 	iterConfig++;
    // }
    _servers.push_back(Server(*iterConfig));
    this->startListen();
    // for (std::vector<std::string>::iterator iter = _config[1].allowedMethods.begin(); iter != _config[1].allowedMethods.end(); iter++)
	// 	std::cout << *iter << std::endl;
}

WebServ::~WebServ()
{}

void WebServ::addToPollfds(struct pollfd *pollfds[], int newfd, int *fdcount, int *fdsize)
{
    // controlla se e raggiunta la dimensione massima dell'array struttura pollfd, in caso, la raddoppia
    if (*fdcount == *fdsize) {
		*fdsize *= 2;
		*pollfds = (struct pollfd *)realloc(*pollfds, sizeof(**pollfds) * (*fdsize));
	}

    // aggiunge la nuova struttura in posizione fdcount
	(*pollfds)[*fdcount].fd = newfd;
	(*pollfds)[*fdcount].events = POLLIN | POLLPRI;
	(*fdcount)++;
}

void WebServ::delFromPollfds(struct pollfd pollfds[], int i, int *fdcount)
{
    // rimuove un pollfds dall'array struttura pollfd
	pollfds[i] = pollfds[*fdcount-1];
	(*fdcount)--;
}

int WebServ::startListen()
{
    std::stack<int>  servStock;
    int fdcount = 0;
    int fdsize;

    for (std::vector<Server>::iterator iter = _servers.begin(); iter != _servers.end(); iter++)
        servStock.push(iter->getSocket());
    fdsize = servStock.size();
    this->_pollfds = (struct pollfd*)malloc(sizeof(*_pollfds) * fdsize);

    for (int i = 0; servStock.size() > 0; i++)
    {
        _pollfds[i].fd = servStock.top();
        _pollfds[i].events = POLLIN;
        servStock.pop();
        fdcount++;
    }

    while (1)
    {
        int poll_count = poll(_pollfds, fdcount, -1);

        if (poll_count == -1)
            die("poll error.");
        
        for (int i = 0; i < fdcount; i++)
        {
            if (_pollfds[i].revents & (POLLIN | POLLPRI | POLLRDNORM))
            {
                for (std::vector<Server>::iterator it = _servers.begin(); it != _servers.end(); it++)
                {
                    if (_pollfds[i].fd == it->getSocket())
                    {
                        int tmpfd = it->acceptConnection(it->getSocket());
                        if (tmpfd != -1)
                            this->addToPollfds(&_pollfds, tmpfd, &fdcount, &fdsize);
                    }
                    else
                    {
                        if (it->handleClient(_pollfds[i].fd) == 1) {
							this->delFromPollfds(_pollfds, i, &fdcount);
						}
                    }
                }
            }
        }
    }

    return 1;
}