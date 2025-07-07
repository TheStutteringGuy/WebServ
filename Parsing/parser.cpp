/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahmed <ahmed@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/04 20:41:13 by ahmed             #+#    #+#             */
/*   Updated: 2025/07/07 14:46:39 by ahmed            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parser.hpp"

Parser::Parser() {

};

Parser::~Parser() {

};

const char *Parser::IncorrArgc::what() const throw()
{
    return "Incorrect number of arguments !!";
}

const char *Parser::ErrorFile::what() const throw()
{
    return "while openning the file !!";
};

const char *Parser::ServerError::what() const throw()
{
    return "No server in top of the config file !!";
};

const char *Parser::ParsError::what() const throw()
{
    return "Expected an open brace after the block !!";
};

const char *Parser::ParsErrorEnd::what() const throw()
{
    return "Expected closed barce at the end of the block !!";
};

const char *Parser::ParsLocation::what() const throw()
{
    return "Expected a Path after the location block !!";
};

const char *Parser::ParsSemiColn::what() const throw()
{
    return "Unexpected semicoln !!";
};

const char *Parser::Unknown::what() const throw()
{
    return "Unknown directive !!";
};

const char *Parser::NoListen::what() const throw()
{
    return "Listen directive is missing !!";
};

const char *Parser::NoRoot::what() const throw()
{
    return "Root directive is missing !!";
};

const char *Parser::InvalidMthode::what() const throw()
{
    return "Invalid HTTP methode !!";
};

void Parser::trim_spaces(std::string &line)
{
    line.erase(0, line.find_first_not_of("\n \t"));
    line.erase(line.find_last_not_of("\n \t") + 1);
};

void Parser::parsToken(const std::string &line)
{
    std::string token;
    char c;

    for (size_t i = 0; i < line.length(); i++)
    {
        c = line[i];
        if (c == '{' || c == '}' || c == ';' || std::isspace(c))
        {
            if (!token.empty())
            {
                tokens.push_back(token);
                token.clear();
            }
            if (c == '{' || c == '}' || c == ';')
            {
                std::string special_char(1, c);
                tokens.push_back(special_char);
            }
        }
        else
        {
            token += c;
        }
    }
    if (!token.empty())
        tokens.push_back(token);
};

const std::map<std::string, Parser::Lexical> &Parser::get_token()
{
    static std::map<std::string, Parser::Lexical> tokens;

    if (tokens.empty())
    {
        tokens["server"] = Parser::SERVER;
        tokens["listen"] = Parser::LISTEN;
        tokens["server_name"] = Parser::SERVER_NAME;
        tokens["host"] = Parser::HOST;
        tokens["root"] = Parser::ROOT;
        tokens["index"] = Parser::INDEX;
        tokens["error_page"] = Parser::ERROR;
        tokens["location"] = Parser::LOCATION;
        tokens["allow_methods"] = Parser::METHODS;
        tokens["autoindex"] = Parser::AUTOINDEX;
        tokens["return"] = Parser::RETURN;
        tokens["cgi_path"] = Parser::CGI_PATH;
        tokens["cgi_ext"] = Parser::CGI_EX;
    }
    return (tokens);
};

Parser::Lexical Parser::get_token_type(const std::string &token)
{
    if (token == "{")
        return (Parser::OPEN_BRACET);
    if (token == "}")
        return (Parser::CLOSED_BRACET);
    if (token == ";")
        return (Parser::SEMICOLN);

    const std::map<std::string, Parser::Lexical> &tokenMap = get_token();
    std::map<std::string, Parser::Lexical>::const_iterator it = tokenMap.find(token);

    if (it != tokenMap.end())
        return (it->second);
    return (Parser::ARGUMENT);
};
const char *Parser::lexical_to_string(Parser::Lexical type)
{
    switch (type)
    {
    case Parser::SERVER:
        return "SERVER";
    case Parser::LISTEN:
        return "LISTEN";
    case Parser::SERVER_NAME:
        return "SERVER_NAME";
    case Parser::HOST:
        return "HOST";
    case Parser::ROOT:
        return "ROOT";
    case Parser::INDEX:
        return "INDEX";
    case Parser::ERROR:
        return "ERROR";
    case Parser::LOCATION:
        return "LOCATION";
    case Parser::METHODS:
        return "METHODS";
    case Parser::AUTOINDEX:
        return "AUTOINDEX";
    case Parser::RETURN:
        return "RETURN";
    case Parser::CGI_PATH:
        return "CGI_PATH";
    case Parser::CGI_EX:
        return "CGI_EX";

    case Parser::OPEN_BRACET:
        return "OPEN_BRACE";
    case Parser::CLOSED_BRACET:
        return "CLOSE_BRACE";
    case Parser::SEMICOLN:
        return "SEMICOLON";
    case Parser::ARGUMENT:
        return "ARGUMENT";
    default:
        return "UNKNOWN";
    }
};

void Parser::Tokenization(int argc, char **argv)
{
    if (argc != 2)
        throw IncorrArgc();

    std::ifstream file(argv[1]);
    if (!file.is_open())
        throw ErrorFile();

    std::string line;
    while (getline(file, line))
    {
        trim_spaces(line);
        if (line[0] == '#')
            continue;
        parsToken(line);
    }
    for (size_t i = 0; i < tokens.size(); i++)
    {
        Lexical type = get_token_type(tokens[i]);
        lexical_vector.push_back(std::make_pair(tokens[i], type));
    }
    // for (size_t i = 0; i < lexical_vector.size(); ++i)
    //     std::cout << lexical_vector[i].first << " -> " << lexical_to_string(lexical_vector[i].second) << std::endl;
    // for (size_t i = 0; i < tokens.size(); i++)
    //     std::cout << tokens[i] << std::endl;
    return;
};

void Parser::parsConfig()
{
    size_t i;

    i = 0;
    while (i < lexical_vector.size())
    {
        if (lexical_vector[i].second == SERVER)
        {
            i++;
            server_blocks.push_back(parsServerBlock(i));
        }
        else
            throw ServerError();
    }
};

ServerBlock Parser::parsServerBlock(size_t &i)
{
    ServerBlock server;

    if (lexical_vector[i].second != OPEN_BRACET)
        throw ParsError();
    i++;
    while (i < lexical_vector.size() && lexical_vector[i].second != CLOSED_BRACET)
    {
        if (lexical_vector[i].second == LOCATION)
            server.locationBlocks.push_back(parsLocationBlock(i));
        else
            server.directives.push_back(parsDirectiveBlock(i));
    }
    if (i >= lexical_vector.size() || lexical_vector[i].second != CLOSED_BRACET)
        throw ParsErrorEnd();
    i++;
    return (server);
};

LocationBlock Parser::parsLocationBlock(size_t &i)
{
    LocationBlock location;

    i++;
    if (lexical_vector[i].second != ARGUMENT)
        throw ParsLocation();
    location.path = lexical_vector[i].first;
    i++;

    if (lexical_vector[i].second != OPEN_BRACET)
        throw ParsError();
    i++;
    while (i < lexical_vector.size() && lexical_vector[i].second != CLOSED_BRACET)
        location.directives.push_back(parsDirectiveBlock(i));
    if (i >= lexical_vector.size() || lexical_vector[i].second != CLOSED_BRACET)
        throw ParsErrorEnd();
    i++;
    return (location);
};

Directive Parser::parsDirectiveBlock(size_t &i)
{
    if (lexical_vector[i].second == SEMICOLN)
        throw ParsSemiColn();

    Directive directive;
    directive.name = lexical_vector[i].first;
    i++;

    while (i < lexical_vector.size() && lexical_vector[i].second == ARGUMENT)
    {
        directive.values.push_back(lexical_vector[i].first);
        i++;
    }

    if (i >= lexical_vector.size() || lexical_vector[i].second != SEMICOLN)
        throw ParsSemiColn();
    i++;
    return (directive);
};
void Parser::validatConfig() const
{
    std::set<std::string> validDirectives;
    validDirectives.insert("listen");
    validDirectives.insert("server_name");
    validDirectives.insert("host");
    validDirectives.insert("root");
    validDirectives.insert("index");
    validDirectives.insert("error_page");
    validDirectives.insert("allow_methods");
    validDirectives.insert("autoindex");
    validDirectives.insert("return");
    validDirectives.insert("cgi_path");
    validDirectives.insert("cgi_ext");

    for (size_t i = 0; i < server_blocks.size(); i++)
    {
        const ServerBlock &server = server_blocks[i];
        bool has_listen = false;
        bool has_root = false;

        for (size_t j = 0; j < server.directives.size(); j++)
        {
            const std::string &name = server.directives[j].name;
            if (validDirectives.find(name) == validDirectives.end())
                throw Unknown();

            if (name == "listen")
                has_listen = true;
            else if (name == "root")
                has_root = true;
        }
        if (!has_listen)
            throw NoListen();
        else if (!has_root)
            throw NoRoot();

        for (size_t y = 0; y < server.locationBlocks.size(); y++)
        {
            const LocationBlock &location = server.locationBlocks[y];
            for (size_t x = 0; x < location.directives.size(); x++)
            {
                const Directive &dir = location.directives[x];

                if (validDirectives.find(dir.name) == validDirectives.end())
                    throw Unknown();
                if (dir.name == "allow_methods")
                {
                    for (size_t k = 0; k < dir.values.size(); k++)
                    {
                        const std::string &method = dir.values[k];
                        if (method != "GET" && method != "POST" && method != "PUT" && method != "HEAD" && method != "DELETE")
                        {
                            throw InvalidMthode();
                        }
                    }
                }
            }
        }
    }
};
// this one for printing from chatgpt
void Parser::printConfig()
{
    for (size_t s = 0; s < server_blocks.size(); ++s)
    {
        std::cout << "Server Block:\n";

        // Print top-level directives
        for (size_t d = 0; d < server_blocks[s].directives.size(); ++d)
        {
            const Directive &dir = server_blocks[s].directives[d];
            std::cout << "  Directive: " << dir.name;
            for (size_t v = 0; v < dir.values.size(); ++v)
                std::cout << " " << dir.values[v];
            std::cout << std::endl;
        }

        // Print location blocks
        for (size_t l = 0; l < server_blocks[s].locationBlocks.size(); ++l)
        {
            const LocationBlock &loc = server_blocks[s].locationBlocks[l];
            std::cout << "  Location: " << loc.path << "\n";

            for (size_t d = 0; d < loc.directives.size(); ++d)
            {
                const Directive &dir = loc.directives[d];
                std::cout << "    Directive: " << dir.name;
                for (size_t v = 0; v < dir.values.size(); ++v)
                    std::cout << " " << dir.values[v];
                std::cout << std::endl;
            }
        }

        std::cout << std::endl;
    }
};