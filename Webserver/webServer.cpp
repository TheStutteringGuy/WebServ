/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webServer.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahmed <ahmed@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/14 15:15:01 by ahmed             #+#    #+#             */
/*   Updated: 2025/07/17 17:41:42 by ahmed            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webServer.hpp"

WebServer::WebServer() {};

WebServer::WebServer(const std::vector<ServerBlock> &servers)
{
    if (!setupListeningSocket(servers))
        std::cerr << "Failed to set up listening socket !!" << std::endl;
};

WebServer::~WebServer()
{
    closeSockets();
};

bool WebServer::setNonBlocking(int fd)
{
    int flag;
    flag = fcntl(fd, F_GETFL, 0);
    if (flag < 0)
        return (false);
    int res;
    res = fcntl(fd, F_SETFL, flag | O_NONBLOCK);
    if (res < 0)
        return (false);
    return (true);
};

bool WebServer::setupListeningSocket(const std::vector<ServerBlock> &servers)
{
    // first here i loop over all my server config
    for (size_t i = 0; i < servers.size(); i++)
    {
        // i create a socket
        int sockFd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockFd < 0)
        {
            std::cerr << "Socket failed : " << strerror(errno) << std::endl;
            continue;
        }
        int option;
        option = 1;
        setsockopt(sockFd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option)); // with this function i tell the OS
        // that is okay to reuse the port even if it is in wait state and all this happend because of this macro SO_REUSEADDR
        // i set the socket to the non_blocking mode
        if (!setNonBlocking(sockFd))
        {
            std::cerr << "The processing failed : " << strerror(errno) << " !!!" << std::endl;
            close(sockFd);
            continue;
        }
        // configure the struct sockaddr_in
        // struct sockaddr_in used to represent the IPV4 socket addresses in the network programming
        struct sockaddr_in addr;
        memset(&addr, 0, sizeof(addr));
        addr.sin_family = AF_INET;                                 // set the address family to IPV4
        addr.sin_port = htons(servers[i].port);                    // set the port and conver it from host byte order to network byte order
        addr.sin_addr.s_addr = inet_addr(servers[i].host.c_str()); // convert IPV4 from string to to network byte, binary format
        // bind socket to the address
        if (bind(sockFd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
        {
            std::cerr << "Bind failed on : " << servers[i].host << ":" << servers[i].port << ": " << strerror(errno) << std::endl;
            close(sockFd);
            continue;
        }
        // start listening for connection
        if (listen(sockFd, 128) < 0)
        {
            std::cerr << "Listening failed : " << strerror(errno) << std::endl;
            close(sockFd);
            continue;
        }
        // store the socket information
        std::cout << "Listening on : " << servers[i].host << ":" << servers[i].port << " fd = ( " << sockFd << " )" << std::endl;
        // i store the info in the listenSockets struct
        ListeningSocket info;
        info.fd = sockFd;
        info.host = servers[i].host;
        info.port = servers[i].port;
        listeningSocket.push_back(info);
        // add to poll
        struct pollfd pfd;
        pfd.fd = sockFd;
        pfd.events = POLLIN; // here We are interested in readable events that is the incoming connections
        pfd.revents = 0;
        pollfds.push_back(pfd);
    }
    if (listeningSocket.empty())
        return (false);
    else
        return (true);
};

void WebServer::run()
{
    mainEventLoop();
};

void WebServer::mainEventLoop()
{
    while (true)
    {
        int res = poll(&pollfds[0], pollfds.size(), -1);
        if (res < 0)
        {
            std::cerr << "Poll failed : " << strerror(errno) << std::endl;
            break;
        }

        size_t n = pollfds.size();
        for (size_t i = 0; i < n; ++i)
        {
            int fd = pollfds[i].fd;
            // Check if this is a listening socket
            bool isListening = false;
            for (size_t j = 0; j < listeningSocket.size(); j++)
            {
                if (fd == listeningSocket[j].fd)
                {
                    isListening = true;
                    break;
                }
            }
            if (isListening)
            {
                if (pollfds[i].revents & POLLIN)
                    handleNewConnections(i);
                continue;
            }
            // Client socket
            if (pollfds[i].revents & POLLIN)
                handleClientRead(fd);
            if (pollfds[i].revents & POLLOUT)
                handleClientWrite(fd);
        }

        // Remove disconnected clients here
        for (size_t i = 0; i < pollfds.size();)
        {
            int fd = pollfds[i].fd;
            bool isListening = false;
            for (size_t j = 0; j < listeningSocket.size(); j++)
                if (fd == listeningSocket[j].fd)
                    isListening = true;
            if (!isListening && clients.count(fd) && clients[fd].disconnect)
            {
                close(fd);
                pollfds.erase(pollfds.begin() + i);
                clients.erase(fd);
            }
            else
                ++i;
        }
    }
}

void WebServer::handleNewConnections(size_t listen_idx)
{
    int listenFd = pollfds[listen_idx].fd;
    while (true)
    {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        int client_fd = accept(listenFd, (struct sockaddr *)&client_addr, &client_len);
        if (client_fd < 0)
        {
            if (errno == EAGAIN || errno == EWOULDBLOCK)
                break;
            std::cerr << "Accept failed : " << strerror(errno) << std::endl;
            break;
        }
        setNonBlocking(client_fd);
        std::cout << "Accepted client : fd( " << client_fd << " ) => from : " << inet_ntoa(client_addr.sin_addr) << ":" << ntohs(client_addr.sin_port) << std::endl;
        struct pollfd client_pfd;
        client_pfd.fd = client_fd;
        client_pfd.events = POLLIN;
        client_pfd.revents = 0;
        pollfds.push_back(client_pfd);
        clients[client_fd] = Client(client_fd);
    }
}

void WebServer::handleClientRead(int fd)
{
    char buf[1024];
    ssize_t n = recv(fd, buf, sizeof(buf), 0);
    if (n <= 0)
    {
        clients[fd].disconnect = true;
        return;
    }
    clients[fd].requestBuffer.append(buf, n);
    if (clients[fd].requestBuffer.find("\r\n\r\n") != std::string::npos)
    {
        if (clients[fd].request.parser(clients[fd].requestBuffer))
        {
            std::cout << "Parsed request: Method=" << clients[fd].request.method
                      << " Path=" << clients[fd].request.path
                      << " Version=" << clients[fd].request.version << std::endl;
            const char *resp = "HTTP/1.1 200 OK\r\nContent-Length: 14\r\nConnection: close\r\n\r\nHello, world!\n";
            clients[fd].responseBuffer = resp;
            clients[fd].responseDone = false;
            // i Switch here to POLLOUT for this fd
            for (size_t i = 0; i < pollfds.size(); ++i)
                if (pollfds[i].fd == fd)
                    pollfds[i].events = POLLOUT;
        }
        else
        {
            // Malformed request
            const char *resp = "HTTP/1.1 400 Bad Request\r\nContent-Length: 11\r\n\r\nBad Request";
            clients[fd].responseBuffer = resp;
            clients[fd].responseDone = false;
            for (size_t i = 0; i < pollfds.size(); ++i)
                if (pollfds[i].fd == fd)
                    pollfds[i].events = POLLOUT;
        }
    }
}

void WebServer::handleClientWrite(int fd)
{
    if (clients[fd].responseDone)
        return;
    ssize_t n = send(fd, clients[fd].responseBuffer.c_str(), clients[fd].responseBuffer.size(), 0);
    if (n <= 0)
    {
        clients[fd].disconnect = true;
        return;
    }
    clients[fd].responseBuffer.erase(0, n);
    if (clients[fd].responseBuffer.empty())
    {
        clients[fd].responseDone = true;
        clients[fd].disconnect = true;
    }
}

void WebServer::closeSockets()
{
    for (size_t i = 0; i < pollfds.size(); i++)
        close(pollfds[i].fd);
    pollfds.clear();
    listeningSocket.clear();
    clients.clear();
}