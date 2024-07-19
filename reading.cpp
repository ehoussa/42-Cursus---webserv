#include "webserv.h"
#include "Server.hpp"

void find_server(Client & client, std::vector<Server> &servs, std::vector<std::pair<int, std::vector<int> > > & common) {
    size_t pos = client.header["Host"].find(":");
    std::string tmp2;
    std::string tmp;
    if (pos == std::string::npos) {
        client.bad_request = 400;
        return ;
    }
    tmp = client.header["Host"].substr(0, pos);
    tmp2 = client.header["Host"].substr(pos + 1);


    if (!isNum(tmp2)) {
        client.bad_request = 400;
        return ;
    }

    if (tmp.length()) {
        for (size_t i = 0; i < common[client.common_index].second.size(); i++) {
            for (size_t j = 0; j < servs[common[client.common_index].second[i]].server_names.size(); j++)
            {
                // << servs[common[client.common_index].second[i]].server_names[j] << std::endl;
                if (servs[common[client.common_index].second[i]].server_names[j] == tmp) {
                    client.serv = &servs[common[client.common_index].second[i]];
                }
            }
        }
    }
    if (client.serv == NULL)
        client.serv = &servs[common[client.common_index].second[0]];
    if (std::atoi(tmp2.c_str()) != client.serv->port_2_tmp) {
        client.bad_request = 400;
        return ;
    }
    
}

void    ft_remove_dup_slash(std::string & str) {
    for (size_t i = 0; i < str.length() - 1; i++) {
        if (str[i] == '/' && str[i + 1] == '/') {
            str.erase(str.begin() + i);
            i = -1;
        }
    }
}

int    ft_check_loction(Server & serv, Client & client) {
    int index_loc = -1;
    size_t len_loc   = 0;

    for (size_t i = 0; i < serv.locations.size(); i++)
    {
        ft_remove_dup_slash(serv.locations[i].Path);
        ft_remove_dup_slash(client.url);
        if (client.url.find(serv.locations[i].Path) == 0)
        {
            if (len_loc < serv.locations[i].Path.length()) {
                index_loc = i;
                len_loc   = serv.locations[i].Path.length();
            }
        }
    }
    if (index_loc != -1) {
        client.loca = &serv.locations[index_loc];

        if (client.loca->Redirect != "") {
            client.bad_request = 302;
            return (1);
        }
    
        if (client.method == "GET" && client.loca->allow_get == 0) {
            client.bad_request = 405;
            return (1);
        }
        else if (client.method == "POST" && client.loca->allow_post == 0) {
            client.bad_request = 405;
            return (1);
        }
        else if (client.method == "DELETE" && client.loca->allow_delete == 0) {
            client.bad_request = 405;
            return (1);
        }
        return (0);
    }
    client.bad_request = 404;
    return (1);
}

int ft_parse_request(Client & client, std::vector<Server> &servs, std::vector<std::pair<int, std::vector<int > > > &common) {

    size_t pos;
    std::string tmp;
    pos = client.request.find("\r\n");
    tmp = client.request.substr(0, pos);
    client.request.erase(0, pos + 2);
    std::stringstream ss(tmp);
    std::getline(ss, client.method, ' ');
    std::getline(ss, client.url, ' ');
    std::getline(ss, tmp, ' ');
    if (client.method != "GET" && client.method != "POST" && client.method != "DELETE") {
        client.bad_request = 501;
        return (0);
    }
    if (tmp != "HTTP/1.1") {
        client.bad_request = 505;
        return (0);
    }
    tmp.clear();
    std::getline(ss, tmp, ' ');
    if (tmp.length()) {
        client.bad_request = 400;
        return (0);
    }
    if (client.url[0] != '/') {
        client.bad_request = 400;
        return (0);
    }

    pos = client.url.find("?");
    if (pos != std::string::npos)
    {
       client.variables = client.url.substr(pos + 1, client.url.length());
       client.url = client.url.substr(0, pos);
    }

    for (size_t i = 0; i < client.url.length() - 1; i++) {
        if (client.url[i] == '.' && client.url[i + 1] == '.') {
            if (client.url[i - 1] == '/' && (i + 2 == client.url.length() || client.url[i + 2] == '/')) {
                client.bad_request = 400;
                return (0);
            }
        }
    }

    while (1) {
        if (client.request.length() == 2) {
            break;
        }
        pos = client.request.find("\r\n");
        tmp = client.request.substr(0, pos);
        size_t pos2 = tmp.find(": ");
        if (pos2 != std::string::npos) {
            client.header[tmp.substr(0, pos2)] = tmp.substr(pos2 + 2);
        }
        client.request.erase(0, pos + 2);
    }
    if (client.serv == NULL) {
        find_server(client, servs, common);
        if (client.bad_request)
            return (0);
    }
    if (!isNum(client.header["Content-Length"])) {
        client.bad_request = 400;
        return (0);
    }

    if (client.method == "POST" && !client.header["Content-Type"].length()) {
        client.bad_request = 400;
        return (0);
    }


    if (client.method == "POST" && !client.header["Content-Length"].length() && client.header["Transfer-Encoding"] != "chunked") {
        client.bad_request = 400;
        return (0);
    }


    if (client.header["Transfer-Encoding"] != "chunked" && client.header["Transfer-Encoding"] != "") {
        client.bad_request = 400;
        return (0);
    }

    client.bad_request = 0;


    if (ft_check_loction(*client.serv, client))
        return (1);

    if (client.loca->root.length()) {
        tmp = client.url.substr(client.loca->Path.length());

        std::string root_ = client.loca->root;
        client.link_get = root_ + tmp;
    } else {
        client.link_get = client.serv->root + client.url;
    }
    if (client.method == "POST" && !client.mime->count(client.header["Content-Type"]) && client.loca->upload) {
        size_t  pos;


        pos = client.header["Content-Type"].find("multipart/form-data; boundary=");
                                                  
        if (pos == std::string::npos) {
            client.bad_request = 415;
            return (0);
        } else {
            for (long unsigned int i = 0; i < client.rest_body.length() &&  client.rest_body[i] != '\r'; i++) {
                client.bound += client.rest_body[i];
            }
        }
    }
    return (0);
}

int    ParseConfigFile::ft_ready_for_read(int fd) {
    if (!map[fd].request_done) {
        int     read_byte = 1;
        char    buf[BUFFER_SIZE];
        size_t pos;


        read_byte = recv(fd, buf, BUFFER_SIZE, 0);
        if (read_byte <= 0) {
            return (1);
        }
        map[fd].request.append(buf, read_byte);
        pos = map[fd].request.find("\r\n\r\n");
        if (pos == std::string::npos) {
            return (0);
        }


        map[fd].request_done = 1;
        map[fd].bad_request = 0;

        map[fd].rest_body = map[fd].request.substr(pos + 4, map[fd].request.length());
        map[fd].request = map[fd].request.substr(0, pos + 4);
        map[fd].readed += map[fd].rest_body.length();

        if (ft_parse_request(map[fd], servs, common) == 1) {
            return (0);
        }

        if  (map[fd].bad_request)
            return 0;

        if (map[fd].method == "POST") {
            if (map[fd].PostMethod())
                return (1);
        }

    } else if (map[fd].method == "POST" && !map[fd].bad_request) {
        if (map[fd].PostMethod() == 1)
            return (1);
    } else {
        int     read_byte = 1;
        char    buf[BUFFER_SIZE];

        read_byte = recv(fd, buf, BUFFER_SIZE, 0);
        if (read_byte <= 0)
            return (1);
    }
    return (0);
}

std::string  to_string(size_t orh) {
    std::stringstream numberStream;
    numberStream << orh;
    return numberStream.str();
}