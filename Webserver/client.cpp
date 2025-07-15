/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahmed <ahmed@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/15 12:21:06 by ahmed             #+#    #+#             */
/*   Updated: 2025/07/15 15:37:50 by ahmed            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "client.hpp"

#include "client.hpp"

Client::Client() : fd(-1), responseDone(false), disconnect(false) {}

Client::Client(int Fd) : fd(Fd), responseDone(false), disconnect(false) {}

Client::~Client() {};