#include "Location.hpp"

int ft_check_start(std::string & str);
int ft_check_end(int found , std::string & str);

Location::Location(std::vector<std::string> & tokens, size_t & i) {
    allow_post = 0;
    allow_get = 0;
    allow_delete = 0;
    auto_index = 0;
    upload = 0;
    cgi = 0;

    if (++i + 1 <= tokens.size()) {
        this->Path = tokens[i];
        i += 2;

        while (1) {
            if (tokens.size() <= i)
                throw "There is something wrong";
            else if (tokens[i] == "}") {
                i++;
                break;
            } else if (tokens[i] == "return" && tokens.size() > i + 2) {
                int found = ft_check_start(tokens[i + 1]);
                Redirect = tokens[i + 1];
                i += ft_check_end(found, tokens[i + 2]);
            } else if (tokens[i] == "upload_path" && tokens.size() > i + 2) {
                int found = ft_check_start(tokens[i + 1]);
                if (upload_path.length())
                    throw "upload_path already exist";
                upload_path = tokens[i + 1];
                i += ft_check_end(found, tokens[i + 2]);
            } else if (tokens[i] == "cgi_path" && tokens.size() > i + 3) {
                int found = ft_check_start(tokens[i + 2]);
                cgi_ex.push_back(tokens[i + 1]);
                cgi_pa.push_back(tokens[i + 2]);
                i++;
                i += ft_check_end(found, tokens[i + 2]);
            } else if (tokens[i] == "allow_methods" && tokens.size() > i + 2) {
                int found = 0;
                for (; ++i < tokens.size();) {
                    if (tokens[i] == ";" && ++found)
                        break;
                    found = ft_check_start(tokens[i]);
                    if (tokens[i] == "GET")
                        allow_get = 1;
                    else if (tokens[i] == "POST")
                        allow_post = 1;
                    else if (tokens[i] == "DELETE")
                        allow_delete = 1;
                    else
                        throw "Unknown method ->" + tokens[i] + "<-";
                    if (found)
                        break;
                }
                if (i >= tokens.size())
                    throw "Some thing wrong in the end of the file";
                i += ft_check_end(found, tokens[i]) - 1;
            } else if (tokens[i] == "root" && tokens.size() > i + 2) {
                int found = ft_check_start(tokens[i + 1]);
                if (root.length())
                    throw "root already exist";
                root = tokens[i + 1];
                i += ft_check_end(found, tokens[i + 2]);
            } else if (tokens[i] == "upload" && tokens.size() > i + 2) {
                int found = ft_check_start(tokens[i + 1]);
                if (tokens[i + 1] == "1" || tokens[i + 1] == "on")
                    upload = 1;
                else if (tokens[i + 1] == "0" || tokens[i + 1] == "off")
                    upload = 0;
                else 
                    throw "On or Off in upload ->" + tokens[i + 1] + "<-";
                i += ft_check_end(found, tokens[i + 2]);
            } else if (tokens[i] == "cgi" && tokens.size() > i + 2) {
                int found = ft_check_start(tokens[i + 1]);
                if (tokens[i + 1] == "1" || tokens[i + 1] == "on")
                    cgi = 1;
                else if (tokens[i + 1] == "0" || tokens[i + 1] == "off")
                    cgi = 0;
                else 
                    throw "On or Off in cgi ->" + tokens[i + 1] + "<-";
                i += ft_check_end(found, tokens[i + 2]);
            } else if (tokens[i] == "autoindex" && tokens.size() > i + 2) {
                int found = ft_check_start(tokens[i + 1]);
                if (tokens[i + 1] == "1" || tokens[i + 1] == "on")
                    auto_index = 1;
                else if (tokens[i + 1] == "0" || tokens[i + 1] == "off")
                    auto_index = 0;
                else 
                    throw "On or Off in autoindex ->" + tokens[i + 1] + "<-";
                i += ft_check_end(found, tokens[i + 2]);
            } else if (tokens[i] == "index" && tokens.size() > i + 2) {
                int found = 0;
                for (;;) {
                    if (++i >= tokens.size())
                        throw "Some thing wrong in the end of the file\n";
                    if (tokens[i] == ";")
                    {
                        found = 1;
                        break;
                    }
                    found = ft_check_start(tokens[i]);
                    index.push_back(tokens[i]);
                    if (found)
                        break;
                }
                i += ft_check_end(found, tokens[i]) - 1;
            } else 
                throw "identifier not found ->" + tokens[i] + "<-";
        }
    }
}


Location::~Location() {

}