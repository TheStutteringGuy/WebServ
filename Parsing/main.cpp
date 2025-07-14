/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahmed <ahmed@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/02 17:50:13 by aahlaqqa          #+#    #+#             */
/*   Updated: 2025/07/14 18:24:13 by ahmed            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parser.hpp"
#include "configProcessor.hpp"
#include "../Webserver/webServer.hpp"

int main(int argc, char **argv)
{
    try
    {
        Parser parser;
        ConfigProcessor processor;

        parser.Tokenization(argc, argv);
        parser.parsConfig();
        parser.validatConfig();

        // parser.printConfig();
        std::vector<ServerBlock> server = parser.getServerBlock();
        processor.processServerBlock(server);

        std::cout << "Configuration processed successfully!" << std::endl;

        WebServer webServ(server);
        webServ.run();
    }
    catch (const std::exception &e)
    {
        std::cout << "Error : " << e.what() << std::endl;
    }

    return 0;
}
