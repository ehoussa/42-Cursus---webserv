#include "Client.hpp"

Client::~Client()
{
	if (file != NULL)
		file->close();
	if (pipe_fd[0] != 0)
		close(pipe_fd[0]);
	if (pid != -1)
		kill(pid, SIGKILL);
	delete file;
}

Client::Client()
{
	serv = NULL;
	request_done = 0;
	pid = -1;
	pipe_fd[0] = 0;
	start_time = time(NULL);
	file = new std::ifstream();
	bad_request = 400;
	client_cgi_on = 0;
	status = 0;
	end_read_cgi = 0;
	if_check = 0;
	m_read = -1;
	readed = 0;
	content_length = -1;
	if_end_of = false;
	loca = NULL;
}

Client::Client(const Client &assign)
{
	if_check = assign.if_check;
	for_the_cgi = assign.for_the_cgi;
	status = assign.status;
	pid = assign.pid;
	end_read_cgi = assign.end_read_cgi;
	files_name = assign.files_name;
	readed = assign.readed;
	if_end_of = assign.if_end_of;
	bad_request = assign.bad_request;
	bound = assign.bound;
	rest_body = assign.rest_body;
	mime = assign.mime;
	content_length = assign.content_length;
	m_read = assign.m_read;
	fd_socket = assign.fd_socket;
	pipe_fd[0] = assign.pipe_fd[0];
	pipe_fd[1] = assign.pipe_fd[1];
	variables = assign.variables;
	header = assign.header;
	file = new std::ifstream();
	link_get = assign.link_get;
	client_cgi_on = assign.client_cgi_on;
	request_done = assign.request_done;
	request = assign.request;
	start_time = assign.start_time;
	common_index = assign.common_index;
	method = assign.method;
	url = assign.url;
	readfd = assign.readfd;
	loca = assign.loca;
	serv = assign.serv;

}

/********     Get Method   **********/

int Client::has_cgi()
{
	for (size_t i = 0; i < this->loca->cgi_ex.size(); i++)
	{
		if (endsWith(this->link_get, this->loca->cgi_ex[i]))
		{
			pipe(this->pipe_fd);
			if ((this->pid = fork()) == 0)
			{ // dupi error l client || protect fork -> 501s 4
				dup2(this->pipe_fd[1], 1);
				close(this->pipe_fd[1]);
				close(this->pipe_fd[0]);
				char **path;
				path = new (char *[3]);
				path[0] = ft_strdup(this->loca->cgi_pa[i].c_str());
				path[1] = ft_strdup(this->link_get.c_str());
				path[2] = NULL;
				char **env;
				int i = 7;
				int j = 0;
				env = new char *[i];
				// REDIRECT_STATUS is used for php-cgi to work properly 
				//  200 is used to tell php-cgi that the request is not a redirection
				// PATH_SCRIPT_NAME and PATH_INFO are used to tell php-cgi the path of the script 
				env[j++] = ft_strdup("REDIRECT_STATUS=200");
				std::string tmp = "SCRIPT_FILENAME=" + this->link_get;
				env[j++] = ft_strdup(tmp);
				tmp = "PATH_INFO=" + this->link_get;
				env[j++] = ft_strdup(tmp);
				tmp = "QUERY_STRING=" + this->variables;
				env[j++] = ft_strdup(tmp);
				env[j++] = ft_strdup("REQUEST_METHOD=GET");
				tmp = "HTTP_COOKIE=" + this->header["Cookie"];
				env[j++] = ft_strdup(tmp);
				env[j++] = NULL;
				execve(path[0], path, env);
				perror("exceve");

				for (int i = 0; env[i]; i++)
					delete[] env[i];
				delete[] env;
				for (int i = 0; path[i]; i++)
					delete[] path[i];
				delete[] path;
				ft_send_error_page_400(fd_socket, *serv, *this);
				throw std::exception();
			}
			close(this->pipe_fd[1]);
			this->start_time = std::time(NULL);
			this->client_cgi_on = 1;
			return (1);
		}
	}
	return (0);
}

int Client::withDirectory()
{
	if (this->url[this->url.size() - 1] != '/')
	{
		int ret;
		std::string response = "HTTP/1.1 301 Moved Permanently\r\nLocation: " + this->url + "/" + "\r\nConnection: close\r\n\r\n";
		ret = send(this->fd_socket, response.c_str(), response.length(), 0);
		if (ret == -1 || ret == 0)
			return 1;
		return (1);
	}

	for (size_t i = 0; i < loca->index.size(); i++)
	{
		std::string index = this->link_get + loca->index[i];
		std::ifstream in;
		this->file->open(index.c_str());
		if (this->file->is_open() && !isDirectory(index.c_str()))
		{
			this->link_get = index;
			this->file->close();
			return (2);
		}
		this->file->close();
	}

	// test server default index file
	std::string index = this->link_get + serv->index;
	std::ifstream in;
	this->file->open(index.c_str());
	if (this->file->is_open() && !isDirectory(index.c_str()))
	{
		this->link_get = index;
		this->file->close();
		return (2);
	}

	if (this->loca->auto_index == 0)
	{
		if (ft_send_error_page_403(this->fd_socket, *serv, *this))
			return (1);
		return (0);
	}

	DIR *dir = opendir(this->link_get.c_str());
	std::string reponse;
	if (dir != NULL)
	{
		struct dirent *e;
		e = readdir(dir);
		while (e != NULL)
		{
			reponse = reponse + "<a href=\"" + e->d_name + "\"";
			reponse = reponse + ">" + e->d_name + "</a><br>";
			e = readdir(dir);
		}
	}
	closedir(dir);

	std::string head = "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: ";
	head += to_string(reponse.length()) + "\n\n";
	if (send(this->fd_socket, head.c_str(), head.length(), 0) <= 0)
		return true;
	if (send(this->fd_socket, reponse.c_str(), reponse.length(), 0) <= 0)
		return true;
	return (1);
}

int Client::GetMethod(fd_set &readfd)
{
	if (this->file->is_open() == 0)
	{
		if (isDirectory(this->link_get.c_str()))
		{
			int ret;
			ret = withDirectory();
			if (ret == 1)
				return 1;
			else if (ret == 0)
				return (0);
		}
		this->file->open(this->link_get.c_str());
		if (!this->file->is_open())
		{
			if (access(this->link_get.c_str(), F_OK) == 0)
			{
				if (ft_send_error_page_403(this->fd_socket, *this->serv, *this))
					return (1);
				return (0);
			}
			if (ft_send_error_page_404(this->fd_socket, *this->serv, *this))
				return (1);
			return (0);
		}
		if (has_cgi())
			return (0);
		std::string ex = ft_file(this->link_get);
		std::string response = "HTTP/1.1 200 OK\r\nContent-Type: " + ex + "\r\nContent-Length: " + to_string(getFileSize(this->link_get.c_str())) + "\r\n";
		response += "Server: webserv\r\nConnection: keep-alive\r\n";
		response += "\r\n";
		if (send(fd_socket, response.c_str(), response.size(), 0) <= 0)
			return (1);
	}
	else if (this->client_cgi_on == 0)
	{
		char buff[SIZE_READ + 1] = {0};
		this->file->read(buff, SIZE_READ);
		buff[SIZE_READ] = '\0';
		int val = this->file->gcount();
		if (val <= 0)
			return (1);
		else
		{
			if (send(fd_socket, buff, val, 0) <= 0)
				return (1);
		}
	}
	else if (this->end_read_cgi == 0)
	{
		if (this->status == 0)
		{
			if (this->start_time + 10 <= std::time(NULL))
			{
				kill(this->pid, SIGKILL);
				pid = -1;
				close(this->pipe_fd[0]);
				this->pipe_fd[0] = 0;
				if (ft_send_error_page_408(fd_socket, *serv, *this)) {
					return (1);
				}
				return (0);
			}
			this->status = waitpid(this->pid, &this->status, WNOHANG);
			return (0);
		}
		if (FD_ISSET(this->pipe_fd[0], &readfd))
		{
			char buf[SIZE_READ];
			int red = read(this->pipe_fd[0], buf, SIZE_READ);
			if (red <= 0)
			{
				this->end_read_cgi = 1;
				close(this->pipe_fd[0]);
				this->pipe_fd[0] = 0;
			}
			this->for_the_cgi.append(buf, red);
		} else {

		}
	}
	else
	{
		size_t pos = this->for_the_cgi.find("\r\n\r\n");
		std::string head;
		std::string body;
		if (pos != std::string::npos)
		{
			head = this->for_the_cgi.substr(0, pos + 2);
			body = this->for_the_cgi.substr(pos + 4, this->for_the_cgi.length());
			if (head.find("HTTP") != 0)
			{
				pos = head.find("Status:");
				if (pos != std::string::npos)
				{
					head = "HTTP/1.1 " + head.substr(pos + 7, head.find("\r\n", pos));
				}
				else
					head = "HTTP/1.1 200 OK\r\n" + head;
			}
			if (head.find("Content-length") == std::string::npos)
			{
				head += "Content-length: ";
				head += to_string(body.length());
				head += "\r\n";
			}
			if (head.find("Content-type") == std::string::npos)
				head += "Content-type: text/html\r\n";
			head += "\r\n";
		}
		else
		{
			body = this->for_the_cgi;
			head = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: ";
			head += to_string(body.length());
			head += "\r\n\r\n";
		}

		int ret;

		ret = send(this->fd_socket, head.c_str(), head.length(), 0);
		if (ret <= 0)
			return (1);
		ret = send(this->fd_socket, body.c_str(), body.length(), 0);
		if (ret <= 0)
			return (1);
		return (1);
	}
	return (0);
}

/********     Delete Method   **********/

int Client::remove_all_dir(std::string const &name)
{
	int i = 0;
	DIR *dir = opendir(name.c_str());
	std::string fullPath = name + "/";
	if (dir != NULL)
	{
		struct dirent *entire;
		while ((entire = readdir(dir)) != NULL)
		{
			if (entire->d_name == std::string(".") || entire->d_name == std::string(".."))
				continue;
			std::string tmp = fullPath + entire->d_name;
			if (isDirectory(tmp.c_str()))
				i += remove_all_dir(tmp);
			i += std::remove(tmp.c_str());
		}
	}
	return (i);
}

void Client::withDirectory_delete()
{
	if (this->url[this->url.size() - 1] != '/')
	{
		ft_send_error_page_409(this->fd_socket, *this->serv, *this);
		return ;
	}
	if (remove_all_dir(this->link_get))
	{
		ft_send_error_page_403(this->fd_socket, *this->serv, *this);
	}
	else if (std::remove(this->link_get.c_str()) == -1)
	{
		ft_send_error_page_403(this->fd_socket, *this->serv, *this);
	}
}

void Client::DeleteMethod()
{
	std::string root_ = this->link_get;
	if (access(root_.c_str(), F_OK) == -1)
	{
		ft_send_error_page_404(this->fd_socket, *this->serv, *this);
		return;
	}
	if (isDirectory(this->link_get.c_str()))
		withDirectory_delete();
	else
		std::remove(this->link_get.c_str());
	ft_send_error_page_204(this->fd_socket, *this->serv, *this);
}

//204   404   403 207