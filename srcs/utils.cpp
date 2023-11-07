#include <iostream>
#include <cstdlib>
#include "genutil.hpp"

void usage()
{
    std::cout << "The correct format is: ./webserver {configuration file}." << std::endl;
    exit (0);
}

int int_error(std::string str)
{
    std::cout << str << std::endl;
    return (0);
}

int die(std::string str)
{
    std::cout << str << std::endl;
    exit (1);
}

std::map<std::string, std::string>::iterator	findKey(std::map<std::string, std::string> &map, std::string key)
{
	for (std::map<std::string, std::string>::iterator iter = map.begin(); iter != map.end(); iter++)
	{
		if ((*iter).first == key)
			return iter;
	}
	return map.end();
}