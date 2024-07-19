#ifndef ParseConfigFile_HPP
#define ParseConfigFile_HPP

class Server;

#include "webserv.h"
#include "Server.hpp"
#include "Location.hpp"

class Client;

class ParseConfigFile {

private:
    std::vector<std::string > tokens;
    fd_set  readfd;
    fd_set  writefd;
    fd_set  errorfd;
    std::vector<Server> servs;
    std::vector<int> clients;
    std::map<int, Client> map;
    int max_fd;
    size_t i;
    std::map<std::string, std::string> mime;
public:

    std::vector<std::pair<int, std::vector<int> > > common;
    ParseConfigFile(std::string const & src);
    void    ParseServer(size_t & i);
    void    ft_check_server();
    void    ft_create_server();


    void ft_creat_server();
    void    MainServer();
    void ft_check_mime();

    int    ft_ready_for_read(int fd);
};


#endif