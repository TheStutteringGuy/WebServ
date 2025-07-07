/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahmed <ahmed@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/02 17:50:13 by aahlaqqa          #+#    #+#             */
/*   Updated: 2025/07/07 14:43:52 by ahmed            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parser.hpp"

int main(int argc, char **argv)
{
    std::string choice;

    try
    {
        Parser parser;

        parser.Tokenization(argc, argv);

        parser.parsConfig();

        parser.validatConfig();

        parser.printConfig();
    }
    catch (const std::exception &e)
    {
        std::cout << "Error : " << e.what() << std::endl;
    };

    return (0);
}
