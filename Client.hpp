#ifndef Client_HPP
#define Client_HPP

#include "webserv.h"

#include "ParseConfigFile.hpp"

class Client
{
private:
    // dyal hossa
    int                         if_check;
    std::ofstream               out;
    // dyali yassi
    std::string     for_the_cgi;
    int             status; // cgi
    pid_t           pid;
    int             end_read_cgi;
public:
    std::vector<std::string>    files_name; // post
    size_t readed;
    bool if_end_of;
    int bad_request;
    // hossa 
    std::string bound;
    std::string rest_body;
    std::map<std::string, std::string>* mime;
    int content_length;
    int m_read;
    // yassir
    int fd_socket;
    int pipe_fd[2];
    std::string variables;
    std::map<std::string, std::string>  header; // map dyal headers m parasya mzyan
    std::ifstream *file;
    std::string link_get;    // root + (url without location)
    int client_cgi_on;
    int request_done;
    std::string request;
    time_t start_time;
    int common_index;
    std::string method;  //   the request method
    std::string url;     //   the requested URL
    fd_set  *readfd;

    Location * loca;
    Server * serv;


    Client();
    Client(const Client& assign);
    void    ft_call_cgi();
    void    DeleteMethod();
    void    withDirectory_delete();
    int     remove_all_dir(std::string const & name);

    int GetMethod(fd_set & readfd);
    int has_cgi(); // method get
    int withDirectory(); // method get

    void    ft_simple_parse();
    void    ft_parse_bound();
    int    ft_open_new_file();
    void    ft_parse_chunked() ;
    bool    ft_extract_hexa();
    void    ft_open_file(std::string ex);
    void    ft_wich_parse();
    int     PostMethod();
    ~Client(); 
};


#endif