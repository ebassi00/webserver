#ifndef GENUTIL_HPP
#define GENUTIL_HPP

#include <iostream>
#include <sstream>
#include <vector>
#include <map>
#include <fstream>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <algorithm>
#include <unistd.h>
#include <dirent.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <time.h>
#include <stack>

typedef	struct	s_location
{
	bool						regex;		// ~
	bool						exact_path;	// =
	std::vector<std::string>	allowedMethods;
	std::string					location;	// path
	std::string					text;		// block
	s_location&	operator=(s_location const & rhs)
	{
		this->regex = rhs.regex;
		this->exact_path = rhs.exact_path;
		this->allowedMethods = rhs.allowedMethods;
		this->location = rhs.location;
		this->text = rhs.text;
		return (*this);
	}
}				t_location;


typedef struct s_config
{
    unsigned short				port;					// listen
	std::string					host;
	std::vector<std::string>	server_name;			// server_name
	std::string					root;					// root
	bool						autoindex;				// autoindex
	std::vector<std::string>	index;					// index
	std::vector<std::string>	errorPages;				// error_page
	unsigned long				client_max_body_size;	// client_max_body_size
	std::vector<std::string>	allowedMethods;
	std::vector<t_location>		locationRules;			// location
	std::vector<std::string>	files;					// files to try
	std::string					cgi_script;
	bool						valid;

    s_config() : port(0)
	{
		root = "";
		host = "0.0.0.0";
		client_max_body_size = 1048576;
		valid = true;
		cgi_script = "";
	}
    
    s_config(s_config const & src)
	{
		port = src.port;
		server_name = src.server_name;
		autoindex = src.autoindex;
		index = src.index;
		errorPages = src.errorPages;
		allowedMethods = src.allowedMethods;
		files = src.files;
		root = src.root;
		host = src.host;
		client_max_body_size = src.client_max_body_size;
		valid = src.valid;
		cgi_script = src.cgi_script;
		locationRules = src.locationRules;
	}

	s_config&	operator=(s_config const & rhs)
	{
		port = rhs.port;
		server_name = rhs.server_name;
		autoindex = rhs.autoindex;
		index = rhs.index;
		errorPages = rhs.errorPages;
		allowedMethods = rhs.allowedMethods;
		files = rhs.files;
		root = rhs.root;
		host = rhs.host;
		client_max_body_size = rhs.client_max_body_size;
		valid = rhs.valid;
		cgi_script = rhs.cgi_script;
		locationRules = rhs.locationRules;
		return (*this);
	}
}   t_config;

typedef struct	s_request
{
	std::string											line;
	std::string											method;
	std::string											path;
	std::string											arguments;
	std::map<std::string, std::string>					headers;
	std::string											body;

	s_request()
	{
		headers.insert(std::make_pair("Host", ""));
		headers.insert(std::make_pair("User-Agent", ""));
		headers.insert(std::make_pair("Accept", ""));
		headers.insert(std::make_pair("Accept-Language", ""));
		headers.insert(std::make_pair("Accept-Encoding", ""));
		headers.insert(std::make_pair("Connection", ""));
		headers.insert(std::make_pair("Upgrade-Insecure-Requests", ""));
		headers.insert(std::make_pair("Sec-Fetch-Dest", ""));
		headers.insert(std::make_pair("Sec-Fetch-Mode", ""));
		headers.insert(std::make_pair("Sec-Fetch-Site", ""));
		headers.insert(std::make_pair("Sec-Fetch-User", ""));
		headers.insert(std::make_pair("Content-Length", ""));
		headers.insert(std::make_pair("Content-Type", ""));
		headers.insert(std::make_pair("Expect", ""));
		headers.insert(std::make_pair("Transfer-Encoding", ""));
		headers.insert(std::make_pair("Cookie", ""));
	};
	s_request& operator=(s_request const & rhs)
	{
		this->line = rhs.line;
		this->method = rhs.method;
		this->path = rhs.path;
		this->headers = rhs.headers;
		this->body = rhs.body;
		return (*this);
	};
	~s_request() { arguments = ""; }
}				t_request;

typedef struct	s_response
{
	std::string											line;
	std::map<std::string, std::string>					headers;
	std::string											body;

	s_response()
	{
		headers.insert(std::make_pair("Server", "webserv"));
		headers.insert(std::make_pair("Date", ""));
		headers.insert(std::make_pair("Content-Type", ""));
		headers.insert(std::make_pair("Content-Length", ""));
		headers.insert(std::make_pair("Last-Modified", ""));
		headers.insert(std::make_pair("ETag", ""));
		headers.insert(std::make_pair("Accept-Ranges", ""));
		headers.insert(std::make_pair("Protocol", ""));
		headers.insert(std::make_pair("Status-Code", ""));
		headers.insert(std::make_pair("Reason-Phrase", ""));
		headers.insert(std::make_pair("Location", ""));
		headers.insert(std::make_pair("Connection", ""));
		headers.insert(std::make_pair("Allow", ""));
		headers.insert(std::make_pair("Set-Cookie", ""));
	}
}				t_response;


typedef struct s_connInfo
{
	int									fd;
	std::string							buffer;
	std::string							headers;
	std::string							path;
	t_config							config;
	int									chunk_size;
	t_request							request;
	t_response							response;
	std::string							body;
	t_location*							location;
	bool								set_cookie;
	std::pair<std::string, std::string>	cookie;
	
	s_connInfo() : chunk_size(-1), set_cookie(false) {};
	s_connInfo(int i) : fd(i), chunk_size(-1), set_cookie(false) {};
}	t_connInfo;

int die(std::string str);
int int_error(std::string str);
void usage();
std::map<std::string, std::string>::iterator	findKey(std::map<std::string, std::string> &map, std::string key);

#endif