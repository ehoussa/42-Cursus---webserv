NAME = webserv
SRC = main.cpp Get.cpp  ParseConfigFile.cpp  Location.cpp memset.cpp \
	 errorPage.cpp Post.cpp Client.cpp reading.cpp Server.cpp

CXX = c++
CPPFLAGS = -Wall -Wextra -Werror  -std=c++98
OBJ = $(SRC:.cpp=.o)

$(NAME) : $(OBJ)
	$(CXX) $(CPPFLAGS) $(OBJ) -o $(NAME)

all : $(NAME)

clean :
	rm -f $(OBJ)

fclean : clean
	rm -f $(NAME)

re : fclean all
