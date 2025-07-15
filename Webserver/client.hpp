/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahmed <ahmed@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/15 12:21:02 by ahmed             #+#    #+#             */
/*   Updated: 2025/07/15 15:27:04 by ahmed            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
#include <unistd.h>

class Client
{
public:
    Client();
    Client(int Fd);
    ~Client();

    int fd;
    std::string responseBuffer;
    std::string requestBuffer;
    bool responseDone;
    bool disconnect;
};

#endif