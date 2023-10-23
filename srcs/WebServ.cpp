#include "../includes/WebServ.hpp"

WebServ::WebServ(char const *filename)  : _filename(filename == NULL ? "default.conf" : filename)
{
    Parser(_filename, _config);
}

WebServ::~WebServ()
{}