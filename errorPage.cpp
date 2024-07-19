#include "webserv.h"
#include "Client.hpp"

int    ft_call_get(Server & serv, Client & cl, int error, std::string _status) {
    (void)serv;
    cl.method = "GET";
    cl.bad_request = 0;
    cl.client_cgi_on = 0;
    if (cl.file->is_open())
        cl.file->close();
    cl.link_get = cl.serv->error_pages[error];
    cl.file->open(cl.link_get.c_str());
    if (!cl.file->is_open())
        return (0);      /// nha send default and close connetion
    std::string ex = ft_file(cl.link_get);
    std::string response = "HTTP/1.1 " + to_string(error) + " " + _status + "\r\nContent-Type: " + ex + "\r\nContent-Length: " + to_string(getFileSize(cl.link_get.c_str())) + "\r\nConnection: keep-alive\r\n\r\n";

    int ret = send(cl.fd_socket, response.c_str(), response.size(), 0);
    if (ret <= 0) {
        return (1);  //  hna just close connection
    }
    return 2;      // don't close connection and don't send any think
}

void    ft_send_one_response(std::string _status, std::string error, int new_socket) {
    std::string body = "<html><head><title>" + error + " - " + _status + "</title><style> body { font-family: Arial, sans-serif; text-align: center; } h1 { color: #555; }p {color: #777;}</style></head><body><h1>" + error + " " + _status + "</h1></body></html>";
    std::string head = "HTTP/1.1 " + error + " " + _status + "\r\nContent-Type: text/html\r\nContent-Length: " + to_string(body.length()) + "\r\n\r\n";
    int ret = send(new_socket , head.c_str() , head.length(), 0);
    if (ret <= 0)
        return ;
    ret = send(new_socket , body.c_str() , body.length(), 0);
    if (ret <= 0)
        return ;
}

bool ft_send_error_page_404(int new_socket, Server & serv, Client & cl) {
    if (serv.error_pages.count(404)) {
        int ret = ft_call_get(serv, cl, 404, "Not Found");
        if (ret == 2)
            return 0;
        if (ret == 1)
            return (true);
    }
    ft_send_one_response("Not Found",to_string(404), new_socket);
    return (true);
}

bool    ft_send_error_page_403(int new_socket, Server & serv, Client & cl) {
    if (serv.error_pages.count(403)) {
        int ret = ft_call_get(serv, cl, 403, "Forbidden");
        if (ret == 2)
            return 0;
        if (ret == 1)
            return (true);
    }
    ft_send_one_response("Forbidden",to_string(403), new_socket);
    return (true);
}

bool    ft_send_error_page_405(int new_socket, Server & serv, Client & cl) {
    if (serv.error_pages.count(405)) {
        int ret = ft_call_get(serv, cl, 405, "Method Not Allowed");
        if (ret == 2)
            return 0;
        if (ret == 1)
            return (true);
    }
    ft_send_one_response("Method Not Allowed",to_string(405), new_socket);
    return (true);

}

bool    ft_send_error_page_409(int new_socket, Server & serv, Client & cl) {
    if (serv.error_pages.count(409)) {
        int ret = ft_call_get(serv, cl, 409, "Conflict");
        if (ret == 2)
            return 0;
        if (ret == 1)
            return (true);
    }
    ft_send_one_response("Conflict",to_string(409), new_socket);
    return (true);
}

bool    ft_send_error_page_204(int new_socket, Server & serv, Client & cl) {
    (void) cl;
    (void) serv;
    std::string response = "HTTP/1.1 204 No Content\r\nContent-Length: 0\r\nConnection: close\r\n\r\n";
    if (send(new_socket, response.c_str(), response.length(), 0) <= 0)
        return true;
    return (true);
}

bool    ft_send_error_page_413(int new_socket, Server & serv, Client & cl) {
    if (serv.error_pages.count(413)) {
        int ret = ft_call_get(serv, cl, 413, "Request Entity Too Large");
        if (ret == 2)
            return 0;
        if (ret == 1)
            return (true);
    }
    ft_send_one_response("Request Entity Too Large", "413", new_socket);
    return (true);
}


bool    ft_send_error_page_500(int new_socket, Server & serv, Client & cl) {
    if (serv.error_pages.count(500)) {
        int ret = ft_call_get(serv, cl, 500, "Internal Server Error");
        if (ret == 2)
            return 0;
        if (ret == 1)
            return (true);
    }
    ft_send_one_response("Internal Server Error", to_string(500), new_socket);
    return (true);
}

bool    ft_send_error_page_400(int new_socket, Server & serv, Client & cl) {

    if (cl.serv->error_pages.count(400)) {
        int ret = ft_call_get(serv, cl, 400, "Bad Request");
        if (ret == 2)
            return 0;
        if (ret == 1)
            return (true);
    }
    ft_send_one_response("Bad Request",to_string(400), new_socket);
    return (true);
}

bool    ft_send_error_page_501(int new_socket, Server & serv, Client & cl) {
    if (serv.error_pages.count(501)) {
        int ret = ft_call_get(serv, cl, 501, "Not Implemented");
        if (ret == 2)
            return 0;
        if (ret == 1)
            return (true);
    }
    ft_send_one_response("Not Implemented", to_string(501), new_socket);
    return (true);
}

bool    ft_send_error_page_415(int new_socket, Server & serv, Client & cl) {
    if (serv.error_pages.count(415)) {
        int ret = ft_call_get(serv, cl, 415, "Unsupported Media Type");
        if (ret == 2)
            return 0;
        if (ret == 1)
            return (true);
    }
    ft_send_one_response("Unsupported Media Type", to_string(415), new_socket);
    return (true);

}

bool    ft_send_error_page_408(int new_socket, Server & serv, Client & cl) {
    if (serv.error_pages.count(408)) {
        int ret = ft_call_get(serv, cl, 408, "Request Timeout");
        if (ret == 2)
            return 0;
        if (ret == 1)
            return (true);
    }
    ft_send_one_response("Request Timeout", to_string(408), new_socket);
    return (true);

}

bool    ft_send_error_page_201(int new_socket, Server & serv, Client & cl) {
    if (serv.error_pages.count(201)) {
        int ret = ft_call_get(serv, cl, 201, "Created");
        if (ret == 2)
            return 0;
        if (ret == 1)
            return (true);
    }
    ft_send_one_response("Created", to_string(201), new_socket);
    return (true);
}

bool    ft_send_error_page_505(int new_socket, Server & serv, Client & cl) {
    if (serv.error_pages.count(505)) {
        int ret = ft_call_get(serv, cl, 505, "HTTP Version Not Supported");
        if (ret == 2)
            return 0;
        if (ret == 1)
            return (true);
    }
    ft_send_one_response("HTTP Version Not Supported", to_string(505), new_socket);
    return (true);
}

void    ft_close_connection(std::map<int, Client> & map, std::vector<int> & clients, int i);

void    ft_switch_it_off(std::map<int, Client> & map, int i, std::vector<int> & clients) {

    std::string response;
    int ret = 0;
    int retur = 1;
    switch (map[clients[i]].bad_request)
    {
        case 400:
            ret = ft_send_error_page_400(clients[i], *map[clients[i]].serv, map[clients[i]]);
            break;
        case 404:
            ret = ft_send_error_page_404(clients[i], *map[clients[i]].serv, map[clients[i]]);
            break;
        case 405:
            ret = ft_send_error_page_405(clients[i], *map[clients[i]].serv, map[clients[i]]);
            break;
        case 413:
            ret = ft_send_error_page_413(clients[i], *map[clients[i]].serv, map[clients[i]]);
            break;
        case 415:
            ret = ft_send_error_page_415(clients[i], *map[clients[i]].serv, map[clients[i]]);
            break;
        case 500:
            ret = ft_send_error_page_500(clients[i], *map[clients[i]].serv, map[clients[i]]);
            break;
        case 501:
            ret = ft_send_error_page_501(clients[i], *map[clients[i]].serv, map[clients[i]]);
            break;
        case 505:
            ret = ft_send_error_page_505(clients[i], *map[clients[i]].serv, map[clients[i]]);
            break;
        case 302:
            ret = 1;
            response = "HTTP/1.1 302 Found\r\nLocation: " + map[clients[i]].loca->Redirect + "\r\nConnection: close\r\n\r\n";
            retur = send(map[clients[i]].fd_socket, response.c_str(), response.length(), 0);
            break;
        default:
        break;
    }
    if (ret == 1 || retur <= 0)
        ft_close_connection(map, clients, i);
}