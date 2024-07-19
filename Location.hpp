#ifndef Location_HPP
#define Location_HPP

#include "webserv.h"

class Location
{
private:
public:
    Location(std::vector<std::string> & tokens, size_t & i);
    ~Location();

    bool                        allow_post;
    bool                        allow_get;
    bool                        allow_delete;
    std::string                 Redirect; // no duplicate
    std::string                 root;     // hada mandatory wa9ila
    bool                        auto_index;
    std::vector<std::string >   index;
    bool                        cgi; // on or off
    std::vector<std::string >   cgi_ex; // vector dyal pair hsn
    std::vector<std::string >   cgi_pa;// cgi khaso ykon khdam m3a Make it work with POST and GET methods
    bool                        upload;// on or off
    std::string                 upload_path;
    std::string                 Path; // hada rah name dyal location
};

#endif