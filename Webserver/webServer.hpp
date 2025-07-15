/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webServer.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahmed <ahmed@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/14 15:15:05 by ahmed             #+#    #+#             */
/*   Updated: 2025/07/15 16:27:12 by ahmed            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERVER_HPP
#define WEBSERVER_HPP

#include <vector>
#include <map>
#include <iostream>
#include <unistd.h>
#include <string>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <cerrno>
#include <arpa/inet.h>
#include <poll.h>
#include "../Parsing/parser.hpp"
#include "client.hpp"

struct ListeningSocket
{
    int fd;
    std::string host;
    int port;
};

class WebServer
{
public:
    WebServer();
    WebServer(const std::vector<ServerBlock> &servers);
    ~WebServer();

    void run();

private:
    std::vector<ListeningSocket> listeningSocket;
    std::vector<struct pollfd> pollfds;
    std::map<int, Client> clients; //like <client_fd, Client>

    bool setupListeningSocket(const std::vector<ServerBlock> &servers);
    bool setNonBlocking(int fd);
    void closeSockets();
    void mainEventLoop();

    void handleNewConnections(size_t listen_idx);
    void handleClientRead(int fd);
    void handleClientWrite(int fd);
};

#endif