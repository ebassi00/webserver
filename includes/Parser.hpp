#ifndef PARSER_HPP
#define PARSER_HPP

#include "genutil.hpp"

class Parser
{
    public:
        Parser(char const *filename, std::vector<t_config> &config);
        ~Parser();
    private:
        Parser();
        void initConfig(std::ifstream &configFile, std::vector<t_config> &config);
        std::string deleteComments(std::string text);
        void checkConfig(std::string text, std::vector<std::string> &serverBlocks);
        size_t checkStart(std::string text);
        size_t checkEnd(std::string text);
        t_config getServerInfo(std::string serverBlock);
        void addRuleToConf(std::string tmp, t_config &conf);
        int parseLocation(std::string &tmp, t_config &conf);
        void fillConf(std::string key, std::string value, t_config &conf);
        void checkHost(std::string value, t_config &conf);
        void checkIfValidIp(std::string value);
        void checkServerName(std::string value, t_config &conf);
        void checkRoot(std::string value, t_config &conf);
        void checkAutoIndex(std::string value, t_config &conf);
        void checkIndex(std::string value, t_config &conf);
        void checkErrorPages(std::string value, t_config &conf);
        void checkClientBodyMaxSize(std::string value, t_config &conf);
        void checkMethods(std::string value, t_config &conf);
        int  alreadyInMethod(std::string value, t_config &conf);
};

#endif