# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: ahmed <ahmed@student.42.fr>                +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/06/02 16:37:10 by aahlaqqa          #+#    #+#              #
#    Updated: 2025/07/15 15:26:26 by ahmed            ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

CXX= c++

CXXFLAGS= -Wall -Wextra -Werror -std=c++98

SOURCES= Parsing/main.cpp Parsing/parser.cpp Parsing/configProcessor.cpp Webserver/webServer.cpp Webserver/client.cpp

OBJECTS= $(SOURCES:.cpp=.o)

NAME= webserv

all: $(NAME)

$(NAME): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(NAME)

.cpp:.o
	$(CXX) -c $(CXXFLAGS) $< -o $@

clean:
	rm -rf $(OBJECTS)

fclean: clean
	rm -rf $(NAME)

re: fclean $(NAME)

.PHONY: clean fclean re all

.SECONDARY: $(OBJECTS)