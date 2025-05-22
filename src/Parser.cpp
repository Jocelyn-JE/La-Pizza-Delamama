/*
** EPITECH PROJECT, 2025
** La-Pizza-Delamama
** File description:
** Parser
*/

#include "Parser.hpp"

#include <iostream>
#include <string>

#include "Utils.hpp"

Parser::Parser(int argc, char **argv)
    : _argc(argc), _cookingMultiplier(0), _cookNb(0), _restockTime(0) {
    for (int i = 0; i < argc; ++i)
        _argv.push_back(argv[i]);
}

double Parser::getCookingMultiplier() const {
    return _cookingMultiplier;
}

unsigned int Parser::getCookNb() const {
    return _cookNb;
}

unsigned int Parser::getRestockTime() const {
    return _restockTime;
}

double Parser::tryConvertDouble(const std::string &arg) {
    try {
        return std::stod(arg);
    } catch (const std::invalid_argument &e) {
        throw Parser::ParserException("Invalid argument: " + arg);
    } catch (const std::out_of_range &e) {
        throw Parser::ParserException("Out of range: " + arg);
    }
}

unsigned int Parser::tryConvertUnsignedInt(const std::string &arg) {
    try {
        if (utils::isNumber(arg) == false)
            throw std::invalid_argument("nan");
        return std::stoul(arg);
    } catch (const std::invalid_argument &e) {
        throw Parser::ParserException("Invalid argument: " + arg);
    } catch (const std::out_of_range &e) {
        throw Parser::ParserException("Out of range: " + arg);
    }
}

bool Parser::parse() {
    if (_argc != 4) {
        std::cout << "Invalid number of arguments" << std::endl;
        return printUsage();
    }
    try {
        _cookingMultiplier = tryConvertDouble(_argv[1]);
        _cookNb = tryConvertUnsignedInt(_argv[2]);
        _restockTime = tryConvertUnsignedInt(_argv[3]);
    } catch (const Parser::ParserException &e) {
        std::cerr << e.what() << std::endl;
        return printUsage();
    }
    if (_cookingMultiplier < 0) {
        std::cout << "Invalid cooking multiplier: " << _argv[1] << std::endl;
        return printUsage();
    }
    if (_cookNb <= 0) {
        std::cout << "Invalid number of cooks: " << _argv[2] << std::endl;
        return printUsage();
    }
    return true;
}

bool Parser::printUsage() const {
    std::cout << "Usage: " << _argv[0]
              << " <cookingMultiplier> <cookNb> <restockTime>" << std::endl
              << "\tcookingMultiplier: Cooking time multiplier (double >= 0)"
              << std::endl
              << "\tcookNb: Number of cooks (unsigned int > 0)" << std::endl
              << "\trestockTime (in ms): Restock time (unsigned int >= 0)"
              << std::endl;
    return false;
}

// ParserException

Parser::ParserException::ParserException(const std::string &errmsg)
    : _errmsg(errmsg) {}

const char *Parser::ParserException::what() const noexcept {
    return _errmsg.c_str();
}
