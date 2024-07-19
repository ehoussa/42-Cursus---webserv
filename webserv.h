#ifndef WEBSERV_H
#define WEBSERV_H



#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sys/socket.h>
#include <fcntl.h>     // open       // mshha mn b3d  
#include <netinet/in.h>
#include <unistd.h>
#include <sys/select.h>
#include <vector>
#include <map>
#include <sstream>
#include <signal.h>
#include <sys/wait.h>
#include <arpa/inet.h>    //close 
#include <sys/types.h>
#include <sys/time.h> //FD_SET, FD_ISSET, FD_ZERO macros 
#include <dirent.h>
#include <ctime>
#include <string>
#include <algorithm>
#include "Client.hpp"
#include <sys/types.h>
#include <signal.h>
#include <iomanip>
#include <cassert>
#include <cstring>


class Client;
class Server;
class ParseConfigFile;
class Location;

#define RESET "\033[0m"
#define GREEN "\033[32;1m"
#define YELLOW "\033[33;1m"
#define BLUE "\033[34;1m"
#define PINK "\033[35;1m"
#define COLOR "\033[36;1m"
#define RED "\033[31;1m"

#define PORT    8080
#define BUFFER_SIZE 1024
#define SIZE_READ BUFFER_SIZE
#define timeout 10


std::string      to_string(size_t  orh);
char	        *ft_strdup(std::string const & s1);
int             isDirectory(const char* path);
void	        *ft_memset(void *b, int c, size_t len);
uint32_t        my_inet_addr(const std::string &host);

int    ft_check_loction(Server & serv, Client & client);


void    ft_switch_it_off(std::map<int, Client> & map, int i, std::vector<int> & clients);

bool                endsWith(const std::string& str, const std::string& suffix);
int                 isFile(const char* path);
size_t              getFileSize(const char* filename);
std::string         ft_file(std::string & link_get);
bool isNum(std::string const & str );


bool    ft_send_error_page_204(int new_socket, Server & serv, Client &);

bool    ft_send_error_page_400(int new_socket, Server & serv, Client &); // 400 Bad Request
bool    ft_send_error_page_403(int new_socket, Server & serv, Client &);
bool    ft_send_error_page_404(int new_socket, Server & serv, Client &);
bool    ft_send_error_page_405(int new_socket, Server & serv, Client &);
bool    ft_send_error_page_408(int new_socket, Server & serv, Client &); // 408 Request Timeout
bool    ft_send_error_page_409(int new_socket, Server & serv, Client &);
bool    ft_send_error_page_415(int new_socket, Server & serv, Client &); // 415 Unsupported Media Type

bool    ft_send_error_page_500(int new_socket, Server & serv, Client &); // 500 Internal Server Error
bool    ft_send_error_page_501(int new_socket, Server & serv, Client &); // 501 Not Implemented
bool    ft_send_error_page_505(int new_socket, Server & serv, Client &); // 505 HTTP Version Not Supported
bool    ft_send_error_page_413(int new_socket, Server & serv, Client &);
bool    ft_send_error_page_201(int new_socket, Server & serv, Client &);

void    ft_send_one_response(std::string _status, std::string error, int new_socket);

#endif