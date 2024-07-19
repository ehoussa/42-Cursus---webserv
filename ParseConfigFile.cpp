#include "Location.hpp"
#include "ParseConfigFile.hpp"
#include "webserv.h"
#include "Client.hpp"

int ft_check_end(int found, std::string &str);
int ft_check_start(std::string &str)
{
    int found = 0;
    if (str[str.size() - 1] == ';' && ++found)
        str.erase(str.size() - 1, 1);
    return (found);
}

ParseConfigFile::ParseConfigFile(std::string const &src)
{
    std::ifstream in(src.c_str());
    if (!in.is_open())
        throw "FileNotFound";
    std::string line;
    while (std::getline(in, line, '\n'))
    {
        line = line.substr(0, line.find('#'));
        std::stringstream str(line);
        std::string word;

        while (str >> word)
            tokens.push_back(word);
    }
    in.close();
    size_t i = 0;
    ParseServer(i);
};

bool isNum(std::string const &str)
{
    size_t i = 0;
    for (; str[i] == ' '; i++)
        ;
    for (; i < str.length(); i++)
        if (!std::isdigit(str[i]))
            return (0);
    return (1);
}

void ParseConfigFile::ParseServer(size_t &i)
{

    Server tmp;
    if (tokens.size() >= i + 3 && tokens[i] == "server" && tokens[i + 1] == "{")
        i += 2;
    else
        throw "Error in config file (syntax error) -> server { ....  }";
    while (1)
    {
        if (tokens.size() <= i)
            throw "Error in config file syntax error -> server { ....  }";
        else if (tokens[i] == "max_client_body_size" && tokens.size() > i + 2)
        {
            int found = ft_check_start(tokens[i + 1]);
            if (!isNum(tokens[i + 1]))
                throw "bad max client body size";
            tmp.max_client_body_size = std::atoi(tokens[i + 1].c_str());
            i += ft_check_end(found, tokens[i + 2]);
        }
        else if (tokens[i] == "host" && tokens.size() > i + 2)
        {
            if (tmp.host.size() != 0)
                throw "You cannot duplicate the host";
            int found = ft_check_start(tokens[i + 1]);
            tmp.host = tokens[i + 1];
            i += ft_check_end(found, tokens[i + 2]);
        }
        else if (tokens[i] == "listen" && tokens.size() > i + 2)
        {
            int found = ft_check_start(tokens[i + 1]);
            if (tmp.port != -1)
                throw "You cannot duplicate the port";
            if (!isNum(tokens[i + 1]))
            {
                throw "The port is not valid " + tokens[i + 1];
            }
            tmp.port = std::atoi(tokens[i + 1].c_str());
            tmp.port_2_tmp = tmp.port;
            i += ft_check_end(found, tokens[i + 2]);
        }
        else if (tokens[i] == "server_names" && tokens.size() > i + 2)
        {
            int found = ft_check_start(tokens[i + 1]);
            tmp.server_names.push_back(tokens[i + 1]);
            i += ft_check_end(found, tokens[i + 2]);
        }
        else if (tokens[i] == "root" && tokens.size() > i + 2)
        {
            if (tmp.root.size() != 0)
                throw "You cannot duplicate the root ";
            int found = ft_check_start(tokens[i + 1]);
            tmp.root = tokens[i + 1];
            if (!isDirectory(tmp.root.c_str()))
                throw "Error when open the root ! ";
            i += ft_check_end(found, tokens[i + 2]);
        }
        else if (tokens[i] == "index" && tokens.size() > i + 2)
        {
            if (tmp.index.size() != 0)
                throw "there is more then one index in the server block\n";
            int found = ft_check_start(tokens[i + 1]);
            tmp.index = tokens[i + 1];
            i += ft_check_end(found, tokens[i + 2]);
        }
        else if (tokens[i] == "error_page" && tokens.size() > i + 3)
        {
            int found = ft_check_start(tokens[i + 2]);
            tmp.error_pages[std::atoi(tokens[i + 1].c_str())] = tokens[i + 2];
            i++;
            i += ft_check_end(found, tokens[i + 2]);
        }
        else if (tokens[i] == "location" && tokens.size() > i + 3 && tokens[i + 2] == "{")
            tmp.locations.push_back(Location(tokens, i));
        else if (tokens[i] == "}")
        {
            i++;
            break;
        }
        else
            throw "identifier not found -> " + tokens[i];
    }
    tmp.check_server();
    servs.push_back(tmp);
    if (tokens.size() - 1 >= i)
    {
        ParseServer(i);
        return; // repeat the function
    }
    ft_check_server();
}

int ft_check_end(int found, std::string &str)
{
    if (found || (str == ";"))
        return 2 + !found;
    else
        throw "there is a missing ;";
}

void ParseConfigFile::ft_create_server()
{
    // Create socket
    for (size_t i = 0; i < common.size(); i++)
    { // loop over ever port in the common
        int socket_fd;
        if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        {
            perror("socket");
            continue;
        }

        struct sockaddr_in serverAddr;
        ft_memset(&serverAddr, 0, sizeof(serverAddr));
        serverAddr.sin_family = AF_INET;

        serverAddr.sin_addr.s_addr = htonl(my_inet_addr(servs[common[i].second[0]].host));
        serverAddr.sin_port = htons(common[i].first);
        int opt = 1;
        setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
        if (bind(socket_fd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
        {
            close(socket_fd);
            perror("bind");
            continue;
        }
        if (listen(socket_fd, -1) < 0)
        {
            close(socket_fd);
            perror("listen");
            continue;
        }

        std::cout << "create one server for " << common[i].first << "\n\n";
        common[i].first = socket_fd;
    }

    MainServer();
}

void ParseConfigFile::ft_check_server()
{
    size_t size = servs.size();
    for (size_t i = 0; i < size - 1; i++)
    {
        if (servs[i].port == -1)
            continue;
        std::pair<int, std::vector<int> > tmp;
        tmp.first = servs[i].port;
        tmp.second.push_back(i);
        for (size_t j = i + 1; j < size; j++)
        {
            if (servs[i].port == servs[j].port && servs[i].host == servs[j].host)
            {
                if (!servs[j].server_names.size() || !servs[i].server_names.size())
                    throw "there is no server_name error !";
                tmp.second.push_back(j);
                servs[j].port = -1;
            }
        }
        if (tmp.second.size() > 1)
        {
            servs[i].port = -1;
            common.push_back(tmp);
        }
    }
    for (long unsigned int i = 0; i < common.size(); i++)
    {
        std::map<std::string, bool> tmp;
        for (long unsigned int j = 0; j < common[i].second.size(); j++)
        {
            for (long unsigned int l = 0; l < servs[common[i].second[j]].server_names.size(); l++)
            {
                if (tmp[servs[common[i].second[j]].server_names[l]] == true || !servs[common[i].second[j]].server_names[l].length())
                {
                    throw "server_name is duplicate";
                }
                tmp[servs[common[i].second[j]].server_names[l]] = true;
            }
        }
    }

    ft_create_server();
}

void ft_close_connection(std::map<int, Client> &map, std::vector<int> &clients, int i);

/*************** Main Server ******************/

void ParseConfigFile::ft_check_mime()
{
    mime["application/x-7z-compressed"] = ".7z";
    mime["application/pdf"] = ".pdf";
    mime["application/vnd.android.package-archive"] = ".apk";
    mime["application/x-apple-diskimage"] = ".dmg";
    mime["application/vnd.apple.installer+xml"] = ".mpkg";
    mime["application/octet-stream"] = ".bin";
    mime["application/x-bittorrent"] = ".torrent";
    mime["application/x-sh"] = ".sh";
    mime["text/x-c"] = ".c";
    mime["text/css"] = ".css";
    mime["text/csv"] = ".csv";
    mime["application/epub+zip"] = ".epub";
    mime["application/x-texinfo"] = ".texinfo";
    mime["image/gif"] = ".gif";
    mime["text/html"] = ".html";
    mime["application/java-vm"] = ".class";
    mime["application/javascript"] = ".js";
    mime["text/x-java-source,java"] = ".java";
    mime["image/jpeg"] = ".jpeg";
    mime["image/x-citrix-jpeg"] = ".jpeg";
    mime["application/vnd.lotus-organizer"] = ".org";
    mime["application/x-mspublisher"] = ".pub";
    mime["application/msword"] = ".doc";
    mime["video/mp4"] = ".mp4";
    mime["application/mp4"] = ".mp4";
    mime["image/png"] = ".png";
    mime["image/x-citrix-png"] = ".png";
    mime["image/x-png"] = ".png";
    mime["image/svg+xml"] = ".svg";
    mime["text/plain"] = ".txt";
    mime["image/webp"] = ".webp";
    mime["application/zip"] = ".zip";
    mime["application/x-tar"] = ".tar";
    mime["application/vnd.rar"] = ".rar";
    mime["application/x-httpd-php"] = ".php";
    mime["audio/mpeg"] = ".mp3";
    mime["application/json"] = ".json";
    mime["image/x-png"] = ".png";
    mime["image/svg+xml"] = ".svg";
    mime["text/plain"] = ".txt";
    mime["image/webp"] = ".webp";
    mime["application/zip"] = ".zip";
    mime["application/x-tar"] = ".tar";
    mime["application/vnd.rar"] = ".rar";
    mime["application/x-httpd-php"] = ".php";
    mime["audio/mpeg"] = ".mp3";
    mime["application/json"] = ".json";
    mime["application/vnd.openxmlformats-officedocument.wordprocessingml.document"] = ".docx";
}

void ParseConfigFile::MainServer()
{
    ft_check_mime();
    signal(SIGPIPE, SIG_IGN);
    ft_creat_server();

    for (size_t i = 0;; i++)
    {
        if (i == servs.size())
        {
            for (i = 0;; i++)
            {
                if (i == common.size())
                    throw "";
                if (common[i].first != -1)
                    break;
            }
            break;
        }
        if (servs[i].socket_id != -1)
        {
            break;
        }
    }

    while (1)
    {
        FD_ZERO(&readfd);
        FD_ZERO(&writefd);
        FD_ZERO(&errorfd);
        max_fd = 0; // achno ghadi yw93 ila 3titih 100000

        for (size_t i = 0; i < servs.size(); i++)
        {
            if (servs[i].port == -1)
                continue;
            FD_SET(servs[i].socket_id, &readfd);
            max_fd = std::max(servs[i].socket_id, max_fd);
        }
        for (size_t i = 0; i < common.size(); i++)
        {
            FD_SET(common[i].first, &readfd);
            max_fd = std::max(common[i].first, max_fd);
        }

        // Add the client sockets to the set
        for (i = 0; i < clients.size(); i++)
        {
            if (clients[i] > 0)
            {
                FD_SET(clients[i], &readfd);
                FD_SET(clients[i], &writefd);
                FD_SET(clients[i], &errorfd);
            }
            if (map[clients[i]].client_cgi_on == 1 && map[clients[i]].pipe_fd[0] != 0)
            {
                FD_SET(map[clients[i]].pipe_fd[0], &readfd);
                max_fd = std::max(map[clients[i]].pipe_fd[0], max_fd);
            }
            max_fd = std::max(clients[i], max_fd);
        }
        // Wait for activity on any of the sockets

        int act = 0;
        while (!act)
            act = select(max_fd + 1, &readfd, &writefd, &errorfd, NULL);
        if (act == -1)
        {
            perror("selec ");
            continue;
        }

        // Check if there's a new incoming connection, accept it
        for (size_t i = 0; i < servs.size(); i++)
        {
            if (servs[i].port == -1)
                continue;
            if (FD_ISSET(servs[i].socket_id, &readfd))
            {
                int client_socket;
                client_socket = accept(servs[i].socket_id, NULL, NULL);
                if (client_socket != -1) {
                    std::cout << "New connection , socket fd is " << client_socket << "\n";
                    map[client_socket].mime = &mime;
                    map[client_socket].readfd = &readfd;
                    // Add the new socket to the client_sockets array
                    map[client_socket].fd_socket = client_socket;

                    map[client_socket].serv = &servs[i];
                    clients.push_back(client_socket);
                }
            }
        }

        for (size_t i = 0; i < common.size(); i++)
        {
            if (FD_ISSET(common[i].first, &readfd))
            {
                int client_socket;
                client_socket = accept(common[i].first, NULL, NULL);
                if (client_socket != -1) {
                    map[client_socket].mime = &mime;
                    std::cout << "New connection Common, socket fd is " << client_socket << std::endl;

                    // Add the new socket to the client_sockets array
                    map[client_socket].readfd = &readfd;
                    map[client_socket].serv = NULL;
                    map[client_socket].fd_socket = client_socket;
                    map[client_socket].common_index = i;
                    clients.push_back(client_socket);
                }
            }
        }

        // Check if any client sockets have activity
        for (i = 0; i < clients.size(); i++)
        {
            if (FD_ISSET(clients[i], &readfd))
            {
                if (ft_ready_for_read(clients[i])) {
                    ft_close_connection(map, clients, i);
                }
            }
            else if (FD_ISSET(clients[i], &writefd) && map[clients[i]].request_done)
            {
                if (map[clients[i]].bad_request)
                {
                    if (map[clients[i]].serv == NULL) {
                        ft_send_one_response("Bad Request",to_string(400), clients[i]);
                        ft_close_connection(map, clients, i);
                    } else
                        ft_switch_it_off(map, i, clients);
                }
                else if (map[clients[i]].method == "POST")
                {
                    if ((((!map[clients[i]].m_read && map[clients[i]].if_end_of) || !map[clients[i]].content_length)))
                    {
                        if ((!map[clients[i]].loca->cgi && !map[clients[i]].loca->upload))
                        {
                            for (std::vector<std::string>::iterator it = map[clients[i]].files_name.begin(); it != map[clients[i]].files_name.end(); ++it)
                            {
                                std::string path = *it;
                                std::remove(path.c_str());
                            }
                            ft_send_error_page_403(map[clients[i]].fd_socket, *map[clients[i]].serv, map[clients[i]]);
                        }
                        else
                        {
                            if (map[clients[i]].serv->max_client_body_size < map[clients[i]].readed)
                            {
                                map[clients[i]].bad_request = 413;
                                for (std::vector<std::string>::iterator it = map[clients[i]].files_name.begin(); it != map[clients[i]].files_name.end(); ++it)
                                {
                                    std::string path = *it;
                                    std::remove(path.c_str());
                                }
                            }
                            if (map[clients[i]].bad_request)
                            {
                                if (map[clients[i]].bad_request == 500)
                                    ft_send_error_page_500(map[clients[i]].fd_socket, *map[clients[i]].serv, map[clients[i]]);
                                else if (map[clients[i]].bad_request == 415)
                                    ft_send_error_page_415(map[clients[i]].fd_socket, *map[clients[i]].serv, map[clients[i]]);
                                else if (map[clients[i]].bad_request == 413)
                                    ft_send_error_page_413(map[clients[i]].fd_socket, *map[clients[i]].serv, map[clients[i]]);
                            }
                            else if (!map[clients[i]].client_cgi_on)
                                ft_send_error_page_201(map[clients[i]].fd_socket, *map[clients[i]].serv, map[clients[i]]);
                        }
                        if (map[clients[i]].method != "GET" && !map[clients[i]].client_cgi_on)
                        {
                            ft_close_connection(map, clients, i);
                        }
                        else if (map[clients[i]].method != "GET")
                            if (map[clients[i]].PostMethod())
                                ft_close_connection(map, clients, i);
                    }
                }
                else if (map[clients[i]].method == "GET")
                {
                    if (map[clients[i]].GetMethod(readfd))  {

                        ft_close_connection(map, clients, i);
                    }
                }
                else if (map[clients[i]].method == "DELETE")
                {
                    map[clients[i]].DeleteMethod();
                    ft_close_connection(map, clients, i);
                }
                else
                {
                    if (ft_send_error_page_501(clients[i], *map[clients[i]].serv, map[clients[i]]))
                        ft_close_connection(map, clients, i);
                }
            }
            else if (FD_ISSET(clients[i], &errorfd))
            {
                ft_close_connection(map, clients, i);
            }
            else if (!map[clients[i]].request_done && map[clients[i]].start_time + timeout <= std::time(NULL))
            {
                ft_close_connection(map, clients, i);
            }
        }
    }
}
