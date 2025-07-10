/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aahlaqqa <aahlaqqa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/02 17:50:13 by aahlaqqa          #+#    #+#             */
/*   Updated: 2025/07/10 17:32:57 by aahlaqqa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parser.hpp"
#include "configProcessor.hpp"

int main(int argc, char **argv)
{
    std::string choice;

    try
    {
        Parser parser;
        ConfigProcessor processor;

        parser.Tokenization(argc, argv);

        parser.parsConfig();

        parser.validatConfig();

        std::vector<ServerBlock> server = parser.getServerBlock();
        processor.processServerBlock(server);

        std::cout << "Configuration precessed successfully !" << std::endl;

        for (size_t i = 0; i < server.size(); ++i)
        {
            const ServerBlock &serv = server[i];
            std::cout << "\n=== Server " << (i + 1) << " ===" << std::endl;
            std::cout << "Port: " << serv.port << std::endl;
            std::cout << "Host: " << serv.host << std::endl;
            std::cout << "Server Name: " << serv.server_name << std::endl;
            std::cout << "Root: " << serv.root << std::endl;
            std::cout << "Index: " << serv.index << std::endl;
            std::cout << "Max Body Size: " << serv.clientSizeBody << " bytes" << std::endl;

            // Print error pages
            for (std::map<int, std::string>::const_iterator it = serv.error_pages.begin();
                 it != serv.error_pages.end(); ++it)
            {
                std::cout << "Error " << it->first << ": " << it->second << std::endl;
            }

            // Print locations
            for (size_t j = 0; j < serv.locationBlocks.size(); ++j)
            {
                const LocationBlock &loc = serv.locationBlocks[j];
                std::cout << "\n  Location: " << loc.path << std::endl;
                std::cout << "    Root: " << loc.root << std::endl;
                std::cout << "    Index: " << loc.index << std::endl;
                std::cout << "    Autoindex: " << (loc.autoindex ? "on" : "off") << std::endl;
                std::cout << "    Methods: ";
                for (size_t k = 0; k < loc.allowed_methods.size(); ++k)
                {
                    std::cout << loc.allowed_methods[k] << " ";
                }
                std::cout << std::endl;

                if (!loc.redirect_url.empty())
                    std::cout << "    Redirect: " << loc.redirect_url << std::endl;

                if (!loc.cgi_path.empty())
                    std::cout << "    CGI Path: " << loc.cgi_path << std::endl;
            }
        }
    }

    // parser.printConfig();
    catch (const std::exception &e)
    {
        std::cout << "Error : " << e.what() << std::endl;
    };

    return (0);
}
