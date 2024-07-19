#include "webserv.h"
#include "Server.hpp"
#include "Client.hpp"
#include "ParseConfigFile.hpp"

uint32_t my_inet_addr(const std::string &host) {
    uint32_t    addr = 0;
    int         shift = 24;

    std::stringstream iss(host);
    std::string part;
    while (std::getline(iss, part, '.')) {
        int num = 0;
        num = std::atoi(part.c_str());
        addr |= num << shift;
        shift -= 8;
    }
    return addr;
}

void ParseConfigFile::ft_creat_server() {
    // Create socket
    for (size_t i = 0; i < servs.size(); i++)
    {
            if (servs[i].port == -1) {
                continue;
            }
            int socket_fd;
            if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
            {
                perror("socket");
                continue;
            }

            struct sockaddr_in   serverAddr;
            ft_memset(&serverAddr, 0, sizeof(serverAddr));
            serverAddr.sin_family = AF_INET;

            serverAddr.sin_addr.s_addr = htonl(my_inet_addr(servs[i].host));
            serverAddr.sin_port = htons(servs[i].port);

            // Set socket options to allow reuse of the address
            int opt = 1;
            // setsockopt - set options on file descriptor
            // SOL_SOCKET - socket level
            // SO_REUSEADDR - reuse of the address
            // &opt - option value
            // sizeof(opt) - size of the option value
            setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

            // Bind the socket to localhost and port 8080
            if(bind(socket_fd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
            {
                close(socket_fd);
                perror("bind");
                continue;
            }

            // Start listening for incoming connections
            if (listen(socket_fd, 0) < -1)
            {
                close(socket_fd);
                perror("listen");
                continue;
            }

            std::cout << "create server for " << servs[i].port << "\n\n";
            servs[i].socket_id = socket_fd;
    }
}

void    ft_close_connection(std::map<int, Client> & map, std::vector<int> & clients, int i) {
    map.erase(clients[i]);
    close(clients[i]);
    clients.erase(clients.begin() + i);
}



int main(int ac, char **av) {

    
    std::string tmp = "default.conf";
    if (ac == 2)
        tmp = av[1];
    try
    {
        ParseConfigFile p(tmp);
    }
    catch(const char * str)
    {
        std::cout << str << std::endl;
        return (3);
    }
    catch(std::string const & str)
    {
        std::cout << str << std::endl;
        return (2);
    }
    catch(...)
    {
        return (3);
    }
    return (0);
}
