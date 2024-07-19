#include "webserv.h"

size_t getFileSize(const char* filename) {
	std::ifstream file(filename, std::ios::binary | std::ios::ate);
	// open file in read mode , offset in the 0 first byte -> std::ios::ate ->  seek to the end 

	if (!file.is_open()) {
		return 0;
	}

	size_t fileSize = file.tellg();  // Get the position which corresponds to the file size  // get position of the offset
	file.close(); // not return size_t ! momkin ikon chi problem hna ?!

	return (fileSize);
}

int isDirectory(const char* path) {
	DIR* dir = opendir(path);

	if (dir == NULL)
		return (0);
	closedir(dir);
	return (1);
}

int isFile(const char* path) {

	std::ifstream file(path);
	int tmp = file.is_open();
	file.close();
	return (tmp);
}


bool endsWith(const std::string& str, const std::string& suffix) {
	if (str.length() < suffix.length()) {
		return false;
	}
	return str.substr(str.length() - suffix.length()) == suffix;
}

std::string ft_file(std::string & link_get) {
	if (isDirectory(link_get.c_str()) == 1)
		return ("text/html");
	if (endsWith(link_get, "html"))
		return ("text/html");
	if (endsWith(link_get, "css"))
		return ("text/css");
	if (endsWith(link_get, "jpg"))
		return ("image/jpg");
	if (endsWith(link_get, "jpeg"))
		return ("image/jpeg");
	if (endsWith(link_get, "png"))
		return ("image/png");
	if (endsWith(link_get, "mp4"))
		return ("video/mp4");
	if (endsWith(link_get, "py"))
		return ("text/html");
	return ("text/html");
}

void	remove_dup(std::string & str) {
	for (size_t i = 0; i < str.size() - 1; i++)
	{
		if (str[i] == '/' && str[i + 1] == '/')
		{
			str.erase(str.begin() + i + 1);
			i--;
		}
	}
}