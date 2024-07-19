#include "Server.hpp"

void	Server::check_server() {
    if (port == -1)
        throw "Please provide at least one port to the server";
    if (host.length() == 0)
        throw "Please provide the host of the server";
    if (root.length() == 0)
        throw "Please provide the root of the server";
    if (port < 0 || port > 65535)
        throw "The port is not valid";
    int		 point = 4; // Start with the highest byte

    std::stringstream iss(host);
    std::string part;
    while (std::getline(iss, part, '.')) {
        int num = 0;
        if (part == "" || !isNum(part))
            throw "The host is not valid " + host;
        num = std::atoi(part.c_str());
        if (num > 255)
            throw "The host is not valid " + host;
        point--; // Move to the next byte
    }
    if (point != 0) 
        throw "The host is not valid " + host;

    for (size_t i = 0; i < locations.size(); i++) {
        if (locations[i].upload && !locations[i].upload_path.size()) {
            throw "errror in upload path";
        }
    }


    std::map<std::string, std::string> tmp;

    for (size_t i = 0; i < locations.size(); i++) {
        if (tmp[locations[i].Path].length()) {
            throw "Duplicate location " + locations[i].Path;
        }
        tmp[locations[i].Path] = locations[i].Path;
    }
}

Server::Server() {
    socket_id = -1;
    max_client_body_size = 4 * 1024 * 1024;
    port = -1;
}