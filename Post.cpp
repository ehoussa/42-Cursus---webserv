#include "webserv.h"

void    Client::ft_wich_parse() {
    if (this->header["Transfer-Encoding"].length()) // just chunked
        this->if_check = 1;
    else if (this->bound.length() && !this->loca->cgi) // just bound
        this->if_check = 2;
    else
        this->if_check = 3;
}

void    Client::ft_open_file(std::string ex) {
    static int name = 1;

    if (this->out.is_open())
        this->out.close();
    if (this->loca->cgi && !this->loca->upload) {
        this->loca->upload_path = "./";
    }
    if (!isDirectory(this->loca->upload_path.c_str()) || !this->loca->upload_path.length())
        this->bad_request = 500;
    else if (!this->bad_request) {
        std::string ext;
        if (ex.length())
            ext = (*mime)[ex];
        else if (!this->loca->cgi)
            ext = (*mime)[this->header["Content-Type"]];
        if (!ext.length() && !this->loca->cgi)
            this->bad_request = 415;
        std::string path(this->loca->upload_path + '/' + to_string(name) + ext);
        while (1) {
            if (access(path.c_str(), F_OK))
                break;
            name++;
            path = this->loca->upload_path + '/' + to_string(name) + ext;
        }
        this->out.open(path.c_str());
        if (!this->out.is_open())
            this->bad_request = 500;
        else
            name++;
        this->files_name.push_back(path);
    }
}

bool is_full(std::string& reset) {
    if (reset.length() < 4)
        return (false);
    size_t pos = reset.find("\r\n");
    if (pos == std::string::npos)
        return (false);
    std::string new_str = reset.substr(pos + 2, reset.length());
    pos = new_str.find("\r\n");
    if (pos == std::string::npos)
        return (false);
    return (true);
}

bool    Client::ft_extract_hexa() {
    std::string hexa;
    size_t      pos;
    char        buf[BUFFER_SIZE];
    size_t      r = 0;

    if (this->rest_body.length() && this->rest_body[0] != '\r' && this->rest_body[1] != '\n')
        this->rest_body = "\r\n" +  this->rest_body;
    if (!is_full(this->rest_body)) {
        r = recv(this->fd_socket, buf, BUFFER_SIZE, 0);
        if(r <= 0) {
            this->content_length = 0;
            this->bad_request = 500;
            return (false);
        }
        this->readed += r;
        this->rest_body.append(buf, r);
        if (!is_full(this->rest_body))
            return (false);
    }
    this->rest_body.erase(0, 2);
    pos = this->rest_body.find("\r\n");
    hexa = this->rest_body.substr(0, pos);
    this->m_read = std::strtol(hexa.c_str(), NULL, 16);
    if (!m_read) {
        this->if_end_of = true;
        return (false);
    }
    this->rest_body.erase(0, pos + 2);
    int len = this->rest_body.length();
    if (len > this->m_read) {
        this->rest_body = this->rest_body.substr(0, this->m_read);
        this->out.write(this->rest_body.c_str(), this->rest_body.size());
        this->rest_body.clear();
        this->if_end_of = true;
        this->m_read = 0;
    } else
        this->m_read -= this->rest_body.length();
    return (true);
}

void    Client::ft_parse_chunked() {
        char    buf[BUFFER_SIZE];
        size_t     r = 0;

        if (!this->out.is_open())
            ft_open_file("");
        if (this->m_read <= 0)
            ft_extract_hexa();
        if (this->m_read) {
            if (this->rest_body.length()) {
                this->out.write(this->rest_body.c_str(), this->rest_body.size());
                this->rest_body.clear();
            }
            if (this->m_read <= BUFFER_SIZE && this->m_read) 
                r = recv(this->fd_socket, buf, this->m_read, 0);
            else if (this->m_read)
                r = recv(this->fd_socket, buf, BUFFER_SIZE, 0);
            if(r <= 0) {
                this->content_length = 0;
                this->bad_request = 500;
                return ;
            }
            this->readed += r;
            this->m_read -= r;
            if (r > 0)
                this->out.write(buf, r);
        }
}

int    Client::ft_open_new_file() {
    size_t  pos;
    char    buf[BUFFER_SIZE];
    int     r;
    std::string new_str;

    pos = this->rest_body.find("\r\n\r\n");
    if (pos == std::string::npos) {
        if (this->content_length > 0) {
            r = recv(this->fd_socket, buf, BUFFER_SIZE, 0);
            if(r <= 0) {
                this->content_length = 0;
                this->bad_request = 500;
                return (0);
            }
            this->readed += r;
            this->content_length -= r;
            this->rest_body.append(buf, r);
        }
    }
    if (this->out.is_open()) {
        pos = this->rest_body.find(this->bound);
        new_str = this->rest_body.substr(0, pos);
        if (new_str.length())
            this->out.write(new_str.c_str(), new_str.size());
        this->rest_body = this->rest_body.substr(pos, this->rest_body.size());
    }
    pos = this->rest_body.find("Content-Type: ");
    if (pos == std::string::npos)
        return (0);
    new_str = this->rest_body.substr(pos + 14, this->rest_body.length());
    pos = new_str.find("\r");
    new_str = new_str.substr(0, pos);
    ft_open_file( new_str);
    pos = this->rest_body.find("\r\n\r\n");
    this->rest_body = this->rest_body.substr(pos + 4, this->rest_body.size());
    pos = this->rest_body.find(this->bound);
    if (pos != std::string::npos)
        return (1);
    return (0);
}

void    Client::ft_parse_bound() {
    size_t          pos;
    char            buf[BUFFER_SIZE];
    std::string new_str;
    int         r;
    int         tr = 1;

    pos = this->rest_body.find(this->bound);
    if (pos != std::string::npos) {
        while (tr)
            tr = ft_open_new_file();
    }
    tr = 1;
    if (this->content_length > 0) {
        r = recv(this->fd_socket, buf, BUFFER_SIZE, 0);
        if(r <= 0) {
                this->content_length = 0;
                this->bad_request = 500;
                return ;
        }
        this->readed += r;
        this->content_length -= r;
        new_str = this->rest_body;
        new_str.append(buf, r);
        pos = new_str.find(this->bound);
        if (pos != std::string::npos) {
            this->rest_body.clear();
            this->rest_body = new_str;
            while (tr)
                tr = ft_open_new_file();
            return ;
        }
        this->rest_body = new_str;
        this->out.write(this->rest_body.c_str(), this->rest_body.size());
        this->rest_body.clear();
    }
}

void    Client::ft_simple_parse() {
    char            buf[BUFFER_SIZE];
    int             r;

    if (!this->out.is_open())
        ft_open_file("");
    if (this->rest_body.length())
        this->out << this->rest_body;
    this->rest_body.clear();
    if (this->content_length) {
        r = recv(this->fd_socket, buf, BUFFER_SIZE, 0);
        if(r <= 0) {
            this->content_length = 0;
            this->bad_request = 500;
        } else {
            this->readed += r;
            this->content_length -= r;
            this->out.write(buf, r);
        }
    }
}

void    Client::ft_call_cgi() {
    char    **env;
    char    **path;
    int         j = 0;
    size_t      i;

    if (this->out.is_open())
        this->out.close();

    for (i = 0; i < loca->cgi_ex.size() ; i++)
	{
		if (endsWith(this->link_get, loca->cgi_ex[i])) {
			pipe(this->pipe_fd);
			if ((this->pid = fork()) == 0) {
                std::freopen(this->files_name[0].c_str(), "r", stdin);
                dup2(this->pipe_fd[1], 1);
				close(this->pipe_fd[1]);
				close(this->pipe_fd[0]);
				path = new (char *[3]);
				path[0] = ft_strdup(loca->cgi_pa[i].c_str());
				path[1] = ft_strdup(this->link_get.c_str());
				path[2] = NULL;
				i = 8;
				env = new char *[i];
				env[j++] = ft_strdup("REDIRECT_STATUS=200");
				env[j++] = ft_strdup("REQUEST_METHOD=POST");
				std::string tmp = "PATH_INFO=" + this->link_get;
				env[j++] = ft_strdup(tmp);
				tmp = "SCRIPT_FILENAME=" + this->link_get;
				env[j++] = ft_strdup(tmp);
                tmp = "CONTENT_TYPE=" + header["Content-Type"];
				env[j++] = ft_strdup(tmp);
                tmp = "CONTENT_LENGTH=" + to_string(getFileSize(this->files_name[0].c_str()));
				env[j++] = ft_strdup(tmp);
                tmp = "HTTP_COOKIE=" + this->header["Cookie"];
				env[j++] = ft_strdup(tmp);
				env[j++] = NULL;
				execve(path[0], path, env);
				perror("exceve");
				for (i = 0; env[i]; i++)
					delete [] env[i];
				delete [] env;
				for (i = 0; path[i]; i++)
					delete [] path[i];
				delete [] path;
				ft_send_error_page_400(fd_socket, *serv, *this);
				throw std::exception();
			}
			close(this->pipe_fd[1]);
			this->start_time = std::time(NULL);
			this->client_cgi_on = 1;
			return ;
		}
	}
    remove(this->files_name[0].c_str());
    this->method = "GET";
}

#include <sys/select.h>

// void

int Client::PostMethod() {
    if (this->pid == -1) {
        if (this->if_check == 0)
            ft_wich_parse();
        if ((this->if_check == 3 || this->if_check == 2) && this->content_length < 0)
            this->content_length = std::atoi(this->header["Content-Length"].c_str()) - this->rest_body.size();
        if (this->if_check == 3) { // no chunked and no bound;
            ft_simple_parse();
        } else if (this->if_check == 1) { // just chunked
            ft_parse_chunked();
        } else if (this->if_check == 2) { // just bound 
            ft_parse_bound();
        }

        if ((!this->content_length || (!this->m_read && this->if_end_of)) && this->loca->cgi) {
            if (isDirectory(this->link_get.c_str())) {
                for (size_t ll = 0; ll < this->loca->index.size(); ll++) {
                    if (!access((this->link_get + this->loca->index[ll]).c_str(), F_OK)) {
                        this->link_get = this->link_get + this->loca->index[ll];
                        break;
                    }
                }
            }
            ft_call_cgi();
        }
    } else {

        if (this->status == 0) {
			if (this->start_time + 10 <= std::time(NULL)) {
                kill(pid, SIGKILL);
                pid = -1;
				close(this->pipe_fd[0]);
				this->pipe_fd[0] = -1;
                remove(this->files_name[0].c_str());
				if (ft_send_error_page_408(this->fd_socket, *this->serv, *this))
					return (1);
                return (0);
			}
			this->status = waitpid(this->pid, &this->status, WNOHANG);
			return (0);
		}
		if (FD_ISSET(this->pipe_fd[0], readfd)) {
			char buf[SIZE_READ];
			int red = read(this->pipe_fd[0], buf, SIZE_READ);
			if (red <= 0)
				close(this->pipe_fd[0]);
			this->for_the_cgi.append(buf, red);
		}
        
		size_t pos = this->for_the_cgi.find("\r\n\r\n");
		std::string head;
		std::string body;
		if (pos != std::string::npos) {
			head = this->for_the_cgi.substr(0, pos + 2);
			body = this->for_the_cgi.substr(pos + 4, this->for_the_cgi.length());
			if (head.find("HTTP") != 0) {
				pos = head.find("Status:");
				if (pos != std::string::npos) {
					head = "HTTP/1.1 " + head.substr(pos + 7, head.find("\r\n", pos)); 
				} else
					head = "HTTP/1.1 200 OK\r\n" + head;
			}
			if (head.find("Content-length") == std::string::npos) {
				head += "Content-length: ";
				head += to_string(body.length());
				head += "\r\n";
			}
			if (head.find("Content-type") == std::string::npos)
				head += "Content-type: text/html\r\n";
			head += "\r\n";
		}
		else {
			body = this->for_the_cgi;
			head = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: ";
			head += to_string(body.length());
			head += "\r\n\r\n";
		}
        remove(this->files_name[0].c_str());
        this->client_cgi_on = 0;
        this->end_read_cgi = -1;
		if (send(fd_socket, head.c_str() , head.length(), 0) <= 0) {
            return (1);
        }
		if (send(fd_socket, body.c_str() , body.length(), 0) <= 0) {
            return (1);
        }
    }
    return (0);
}
