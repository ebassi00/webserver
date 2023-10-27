#include "../includes/Parser.hpp"

Parser::Parser(char const *filename, std::vector<t_config> &config)
{
    std::ifstream configFile(filename);

    if (!configFile.is_open())
        die("Impossible to open .conf file!");
    
    initConfig(configFile, config);
    configFile.close();
}

Parser::~Parser()
{}

std::string Parser::deleteComments(std::string text)
{
	std::string	temp;
	size_t		pos;
	size_t		nl;

	temp = text;
	while (text.find('#') != std::string::npos)
	{
		pos = text.find('#');
		nl = text.find('\n', pos);
		if (nl == std::string::npos)
			return (text.substr(0, pos));
		else
		{
			temp = text.substr(0, pos) + text.substr(nl);
			text = temp;
		}
	}
	return (temp);
}

size_t Parser::checkStart(std::string text)
{
    size_t	i = 6 + text.find("server");
	while (text.at(i) == ' ' || text.at(i) == '\n' || text.at(i) == '\t')
		i++;
	if (text.at(i) != '{')
		die("Server block must start with 'server {'.");
	return (i + 1);
}

size_t Parser::checkEnd(std::string text)
{
    std::string::iterator	iter = text.begin();
	int						count = 0;

	while (iter != text.end())
	{
		if (*iter == '{')
			count++;
		else if (*iter == '}')
			count--;
		if (count == 0 && *iter == '}')
			return (std::distance(text.begin(), iter) - 1);
		iter++;
	}
	die("Server blocks must end with '}'.");
	return (0);
}

void Parser::checkConfig(std::string text, std::vector<std::string> &serverBlocks)
{
    size_t start, end = 0;

    while (text.find("server") != std::string::npos)
    {
        start = checkStart(text);
        end = checkEnd(text);
        serverBlocks.push_back(text.substr(start, end - start));
        text = text.substr(0, text.find("server")) + text.substr(text.find("}", end + 1) + 1);
    }

	if (text.find_first_not_of(" \t\n") != std::string::npos)
		die("All configuration must be inside server blocks.");
}

int	Parser::parseLocation(std::string &tmp, t_config &conf)
{
	t_location	loc;
	size_t		start;
	size_t		end;
	(void)conf;

	tmp = tmp.substr(8).substr(tmp.find_first_not_of(" \t"));
	tmp = tmp.substr(tmp.find_first_not_of(" \t"));
	loc.regex = false;
	loc.exact_path = false;

	if (tmp.at(0) == '~')
		loc.regex = true;
	else if (tmp.at(0) == '=')
		loc.exact_path = true;

	start = tmp.find_first_not_of(" \t~=");
	tmp = tmp.substr(start);
	end = tmp.find_first_of(" \t\n{");
	loc.location = tmp.substr(0, end);
	if (loc.location.empty())
		die("There's no path defined in location.");

	tmp = tmp.substr(tmp.find_first_of(" \t\n{"));
	start = tmp.find_first_not_of(" \t\n{");
	end = tmp.find_last_not_of(" \t\n}");
	loc.text = tmp.substr(start, end - start + 1);

	conf.locationRules.push_back(loc);
	return (0);
}

void Parser::checkIfValidIp(std::string value)
{
	std::string::iterator	iter = value.begin();
	int						dots = 0;
	int						numbers;

	if (!std::isdigit(*iter))
		die("IP must be of valid form.");
	numbers = 1;
	while (*iter && std::isdigit(*iter))
		iter++;
	while (iter != value.end())
	{
		if (*iter && std::isdigit(*iter))
		{
			numbers++;
			while (*iter && std::isdigit(*iter))
				iter++;
		}
		if (*iter && *iter == '.')
			dots++;
		if (*iter && *iter == ':')
			break ;
		iter++;
	}
	if (dots == 3 && numbers == 4)
		return ;
	die("IP must be of valid form.");
}

void Parser::checkHost(std::string value, t_config &conf)
{
	std::string::iterator	it = value.begin();
	long	ip;
	long	port;

	if (value.find_first_of(" \t") != std::string::npos)
		die("There can't be spaces in host:port declaration.");

	while (it != value.end())
	{
		if (!std::isdigit(*it) && *it != '.' && *it != ':')
			die("Format not accepted!");
		it++;
	}

	if (value.find_first_of('.') != std::string::npos)
	{
		checkIfValidIp(value);

		for (int i = 0; i < 4; i++)
		{
			ip = strtol(value.c_str(), NULL, 0);
			if (ip > 255 || ip < 0)
				die("IP must be between 0 and 255");
			std::cout << value;
			conf.host += value.substr(0, value.find_first_of('.'));
			if (i != 3)
				conf.host += ".";
			value.substr(value.find_first_of('.') + 1);
		}
		it = value.begin();
		while (*it && std::isdigit(*it))
			it++;
		if (*it != ':')
			die("IP must contain semicolon");
		if (it++ == value.end())
			die("'listen' directive must specify a port.");
		if (!std::isdigit(*it))
			die("IP must be of valid form.");
		value = value.substr(value.find_first_of(':') + 1);
	}

	// controlla se la porta e corretta
	port = strtol(value.c_str(), NULL, 0);
	if (port > 65535 || ip < 0)
		die("Port number must be unsigned short.");
	conf.port = (unsigned short) port;
}

void Parser::checkServerName(std::string value, t_config &conf)
{
	std::string tmp;
	size_t		index;

	index = value.find_first_of(" \t");
	if (index == std::string::npos)
		tmp = value;
	else
	{
		while (index != std::string::npos)
		{
			tmp = value.substr(0, index);
			value = value.substr(index);
			value = value.substr(value.find_first_not_of(" \t"));
			conf.server_name.push_back(tmp);
			index = value.find_first_of(" \t");
		}
		tmp = value.substr(0, index);
	}
	conf.server_name.push_back(tmp);

	std::vector<std::string>::iterator	iter = conf.server_name.begin();
	while (iter != conf.server_name.end())
	{
		if ((*iter).find_first_of(".") == std::string::npos)
			die("Each domain must have at least one dot.");
		tmp = *iter;
		while (tmp.find_first_of(".") != std::string::npos)
		{
			tmp = tmp.substr(tmp.find_first_of("."));
			if (tmp.at(1) == '\0' || tmp.at(1) == '.')
				die("Server name is of invalid format.");
			tmp = tmp.substr(1);
		}
		iter++;
	}
}

void Parser::checkRoot(std::string value, t_config &conf)
{
	if (value.find(" \n\t") != std::string::npos)
		die("Root path can only be one.");
	conf.root = value;
}

void Parser::checkAutoIndex(std::string value, t_config &conf)
{
	if (value.compare("on") && value.compare("off"))
		die("Valid values for autoindex are on|off.");
	if (!value.compare("on"))
		conf.autoindex = true;
	else
		conf.autoindex = false;
}

void Parser::checkIndex(std::string value, t_config &conf)
{
	std::string tmp;

	while (value.find_first_not_of(" \t\n") != std::string::npos)
	{
		tmp = value.substr(0, value.find_first_of(" \t"));
		conf.index.push_back(tmp);
		if (value.find_first_of(" \t") == std::string::npos)
			break ;
		value = value.substr(value.find_first_of(" \t"));
		value = value.substr(value.find_first_not_of(" \t"));
	}
}

void Parser::checkErrorPages(std::string value, t_config &conf)
{
	std::string tmp;

	while (value.find_first_not_of(" \t \n") != std::string::npos)
	{
		tmp = value.substr(0, value.find_first_of(" \t"));
		conf.errorPages.push_back(tmp);
		if (value.find_first_of(" \t") == std::string::npos)
			break ;
		value = value.substr(value.find_first_of(" \t"));
		value = value.substr(value.find_first_not_of(" \t"));
	}
}

void Parser::checkClientBodyMaxSize(std::string value, t_config &conf)
{
	size_t idx = value.find_first_not_of("0123456789");
	unsigned long size;
	size = strtoul(value.c_str(), NULL, 0);
	if (idx != std::string::npos)
	{
		if (value.c_str()[idx + 1] != '\0')
			die("Write client_max_body_size in an acceptable way.");
		if (value.at(idx) == 'K')
			size *= 1024;
		else if (value.at(idx) == 'M')
			size *= (1024 * 1024);
		else if (value.at(idx) == 'B')
			;
		else
			die("Correct units for client_max_body_size are B (bit), K (kilobit), M (megabit).");
	}
	conf.client_max_body_size = size;
}

int Parser::alreadyInMethod(std::string value, t_config &conf)
{
	for (std::vector<std::string>::iterator iter = conf.allowedMethods.begin(); iter != conf.allowedMethods.end(); iter++)
		if (value == *iter)
			return (1);
	return (0);
}

void Parser::checkMethods(std::string value, t_config &conf)
{
	std::string methods[5] = {"GET", "POST", "PUT", "DELETE", "HEAD"};
	std::string tmp;
	int i;

	if (value.find_first_of(" \t\n") != std::string::npos)
	{
		while (value.length())
		{
			tmp = value.substr(0, value.find_first_of(" \t\n"));
			for (i = 0; i < 5; i++)
			{
				if (value.compare(methods[i]) && !alreadyInMethod(value, conf))
				{
					conf.allowedMethods.push_back(tmp);
					break ;
				}
			}
			if (i == 5)
				die("Method not recognized, allowed methods are: GET, POST, PUT, DELETE, HEAD.");
			if (value.find_first_of(" \t\n") == std::string::npos)
				break ;
			value = value.substr(value.find_first_of(" \t\n"));
			value = value.substr(value.find_first_not_of(" \t\n"));
		}
	}
	else
	{
		for (i = 0; i < 5; i++)
		{
			if (value.compare(methods[i]) && !alreadyInMethod(value, conf))
				conf.allowedMethods.push_back(value);
		}
		if (i == 5)
			die("Method not recognized, allowed methods are: GET, POST, PUT, DELETE, HEAD.");
	}
}

void Parser::fillConf(std::string key, std::string value, t_config &conf)
{
	int	i;
	std::string	rule[9] = { "listen", "server_name", "root", "autoindex", "index", "error_page", "client_max_body_size", "allowed_methods", "location" };

	for (i = 0; i < 9; i++)
		if (!rule[i].compare(key))
			break ;
	if (i == 9)
		die("Rule" + key + "not supported!");
	
	switch (i)
	{
		case 0: // listen
			checkHost(value, conf); break ;
		case 1: // server_name
			checkServerName(value, conf); break ;
		case 2: // root
			checkRoot(value, conf); break ;
		case 3: // auto_index
			checkAutoIndex(value, conf); break ;
		case 4: // index
			checkIndex(value, conf); break ;
		case 5: // error_page
			checkErrorPages(value, conf); break ;
		case 6: // client_max_body_size
			checkClientBodyMaxSize(value, conf); break ;
		case 7:
			checkMethods(value, conf); break ;
		case 8:
			break ;
		default:
			die("Unrecognized rule.");
	}
}

void Parser::addRuleToConf(std::string tmp, t_config &conf)
{
	std::string	key;
	std::string	value;
	(void)conf;

	if (tmp.find_first_not_of(" \t") == std::string::npos)
		die("Key and Value must be separated by whitespace!");
	
	key = tmp.substr(0, tmp.find_first_of(" \t"));
	if (!key.compare("location"))
		parseLocation(tmp, conf);
	else
	{
		value = tmp.substr(key.length());
		value = value.substr(value.find_first_not_of(" \t"));
	}
	fillConf(key, value, conf);
	
}

t_config Parser::getServerInfo(std::string serverBlock)
{
    t_config	ret;
	std::string	tmp;
	ssize_t		start;
	ssize_t		end;

	end = -1;
	while (1)
	{
		if (end >= static_cast<ssize_t>(serverBlock.length()))
			break ;
		serverBlock = serverBlock.substr(end + 1);
		if (serverBlock.empty())
			break ;
		start = serverBlock.find_first_not_of(" \n\t");
		if (!std::strncmp(serverBlock.substr(start).c_str(), "location", 8))
			end = serverBlock.find("}") + 1;
		else
			end = serverBlock.find(";");
		tmp = serverBlock.substr(start, end - start);
		if (std::strncmp(tmp.c_str(), "location", 8) && tmp.find("\n") != std::string::npos)
			die("Rules must end with semicolon.");
		addRuleToConf(tmp, ret);
	}

	if (!serverBlock.empty())
		serverBlock = serverBlock.replace(serverBlock.begin() + start, serverBlock.begin() + start + tmp.length(), "");

	if (ret.port == 0 || ret.root == "")
		die("Server must listen to a port and be rooted in a path.");

	return (ret);
}

void Parser::initConfig(std::ifstream &configFile, std::vector<t_config> &config)
{
    std::string                 text;
    std::stringstream           buffer;
    std::vector<std::string>    serverBlocks;

    buffer << configFile.rdbuf();
    text = buffer.str();

    text = deleteComments(text);

    checkConfig(text, serverBlocks);

    std::vector<std::string>::iterator it = serverBlocks.begin();

    while (it != serverBlocks.end())
    {
        config.push_back(getServerInfo(*it));
        it++;
    }

	// for (std::vector<std::string>::iterator iter = config[1].files.begin(); iter != config[1].files.end(); iter++)
	// 	std::cout << *iter << std::endl;
}