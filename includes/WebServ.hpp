#ifndef WEBSERV_HPP
#define WEBSERV_HPP

#include "Server.hpp"
#include "Parser.hpp"
#include "genutil.hpp"
#include "poll.h"

class Server;

class WebServ
{
    public:
        WebServ(const char *filename);
        ~WebServ();
        struct pollfd   *_pollfds;
    private:
        WebServ();
        const char				*_filename;
        std::vector<t_config>	_config;
        std::vector<Server>     _servers;
        int startListen();
        void addToPollfds(struct pollfd *pollfds[], int newfd, int *fdcount, int *fdsize);
        void delFromPollfds(struct pollfd pollfds[], int i, int *fdcount);
};

#endif