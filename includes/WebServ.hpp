#ifndef WEBSERV_HPP
#define WEBSERV_HPP

#include "Server.hpp"
#include "Parser.hpp"

class Server;

class WebServ
{
    public:
        WebServ(const char *filename);
        ~WebServ();
    private:
        WebServ();
        const char				*_filename;
        std::vector<t_config>	_config;
};

#endif