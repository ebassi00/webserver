NAME = webserv
CC		=	c++
FLAGS	=	-Wall -Werror -Wextra -std=c++98 -I./includes -g
SRC		=	main.cpp \
			Server.cpp \
			WebServ.cpp \
			Parser.cpp \
			utils.cpp
SRCS	=	$(addprefix srcs/, $(SRC))
OBJS	=	$(SRCS:.cpp=.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(FLAGS) $(OBJS) -o $(NAME)

%.o: %.cpp
	$(CC) -c $(FLAGS) $< -o $@

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re