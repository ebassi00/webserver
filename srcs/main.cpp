#include "../includes/WebServ.hpp"

int main(int argc, char **argv)
{
    if (argc != 2)
        usage();

    // start webserver..
    WebServ webserv(argv[1]);
    return (0);
}