#ifndef SERVER_HPP
#define SERVER_HPP

#include "webserv.h"

class Location;


class Server {

public:
	int							port;
	int							port_2_tmp;
	std::string					host;
	std::vector<std::string >	server_names;
	std::map<int, std::string>	error_pages;
	size_t						max_client_body_size;
	std::vector<Location>		locations;

	int					socket_id;
	std::string			root;
	std::string			index;
	Server();
	void	check_server();
};


#endif