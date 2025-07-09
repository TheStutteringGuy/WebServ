/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   configProcessor.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aahlaqqa <aahlaqqa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/01 16:54:24 by ahmed             #+#    #+#             */
/*   Updated: 2025/07/10 00:40:36 by aahlaqqa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "configProcessor.hpp"

ConfigProcessor::ConfigProcessor() {

};

ConfigProcessor::~ConfigProcessor() {

};

const char *ConfigProcessor::InvalidPortException::what() const throw()
{
    return "Invalid Port number !!";
};

const char *ConfigProcessor::DuplicatePortEXception::what() const throw()
{
    return "Duplicate Port detected !!";
};

const char *ConfigProcessor::InvalidPathException::what() const throw()
{
    return "Invalid Path detected !!";
};

const char *ConfigProcessor::InvalidDirectiveException::what() const throw()
{
    return "Invalid directive is detected !!";
};

void ConfigProcessor::processServerBlock(std::vector<ServerBlock> &blocks)
{
    for (size_t i = 0; i < blocks.size(); i++)
    {
        setDefaultValue(blocks[i]);
        processServerDirective(blocks[i]);
        for (size_t j = 0; j < blocks[i].locationBlocks.size(); j++)
        {
            setLocation(blocks[i].locationBlocks[j], blocks[i]);
            processLocationDirective(blocks[i].locationBlocks[j], blocks[i]);
        }
    }
    validateConfig(blocks);
};

void ConfigProcessor::processServerDirective(ServerBlock &blocks)
{
    for (size_t i = 0; i < blocks.directives.size(); i++)
    {
        const Directive &dir = blocks.directives[i];
        if (dir.name == "listen")
        {
            if (!dir.values.empty())
                blocks.port = stringToInt(dir.values[0]);
        }
        else if (dir.name == "host")
        {
            if (!dir.values.empty())
                blocks.host = dir.values[0];
        }
        else if (dir.name == "server_name")
        {
            if (!dir.values.empty())
                blocks.server_name = dir.values[0];
        }
        else if (dir.name == "root")
        {
            if (!dir.values.empty())
                blocks.root = dir.values[0];
        }
        else if (dir.name == "index")
        {
            if (!dir.values.empty())
                blocks.index = dir.values[0];
        }
        else if (dir.name == "error_page")
        {
            if (dir.values.size() >= 2)
            {
                int err_code = stringToInt(dir.values[0]);
                blocks.error_pages[err_code] = dir.values[1];
            }
        }
        else if (dir.name == "client_max_body_size")
        {
            if (!dir.values.empty())
                blocks.clientSizeBody = static_cast<size_t>(stringToInt(dir.values[0]));
        }
    }
};

void ConfigProcessor::processLocationDirective(LocationBlock &location, const ServerBlock &server)
{
    for (size_t i = 0; i < location.directives.size(); i++)
    {
        const Directive &dir = location.directives[i];
        if (dir.name == "root")
        {
            if (!dir.values.empty())
                location.root = dir.values[0];
        }
        else if (dir.name == "index")
        {
            if (!dir.values.empty())
                location.index = dir.values[0];
        }
        else if (dir.name == "allow_methods")
            location.allowed_methods = dir.values;
        else if (dir.name == "autoindex")
        {
            if (!dir.values.empty())
                location.autoindex = (dir.values[0] == "on");
        }
        else if (dir.name == "return")
        {
            if (!dir.values.empty())
                location.redirect_url = dir.values[0];
        }
        else if (dir.name == "cgi_path")
        {
            if (!dir.values.empty())
                location.cgi_path = dir.values[0];
        }
        else if (dir.name == "cgi_ext")
            location.cgi_extention = dir.values;
    }
};

void ConfigProcessor::validateConfig(const std::vector<ServerBlock> &blocks)
{
    validatePort(blocks);
    validatePath(blocks);
};

void ConfigProcessor::validatePort(const std::vector<ServerBlock> &blocks)
{
    std::set<int> nonused_port;
    for (size_t i = 0; i < blocks.size(); i++)
    {
        if (!isValidPort(blocks[i].port))
            throw InvalidPortException();
        if (nonused_port.find(blocks[i].port) != nonused_port.end())
            throw DuplicatePortEXception();
        nonused_port.insert(blocks[i].port);
    }
};

void ConfigProcessor::validatePath(const std::vector<ServerBlock> &blocks)
{
    for (size_t i = 0; i < blocks.size(); i++)
    {
        const ServerBlock &server = blocks[i];
        if (!existDirectory(server.root))
            throw InvalidPathException();
        for (std::map<int, std::string>::const_iterator it = server.error_pages.begin(); it != server.error_pages.end(); it++)
        {
            std::string fullPath = handlePath(server.root, it->second);
            if (!existFile(fullPath))
                throw InvalidPathException();
        }
        for (size_t j = 0; j < server.locationBlocks.size(); j++)
        {
            const LocationBlock &location = server.locationBlocks[i];
            if (!existDirectory(location.root))
                throw InvalidPathException();
        }
    }
};

void ConfigProcessor::setDefaultValue(ServerBlock &server)
{
    if (server.port == 80 && server.host == "0.0.0.0")
    {
        // guys this is a do nothing block because i already set it in the serverblock constructor :)
    }
    if (server.server_name.empty())
        server.server_name = "localhost";
    if (server.index.empty())
        server.index = "index.html";
};