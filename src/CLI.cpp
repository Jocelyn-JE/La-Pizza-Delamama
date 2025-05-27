/*
** EPITECH PROJECT, 2025
** La-Pizza-Delamama
** File description:
** CLI
*/

#include "CLI.hpp"

#include <iostream>
#include <string>

#include "Utils.hpp"

plazza::CLI::CLI(
    double cookingMultiplier, unsigned int cookNb, unsigned int restockTime)
    : _reception(cookingMultiplier, cookNb, restockTime) {}

void plazza::CLI::runInterface() {
    while (true) {
        printPrompt();
        std::getline(std::cin, _lastInput);
        handleInput(_lastInput);
    }
}

void plazza::CLI::printPrompt() {
    std::cout << _prompt;
}

void plazza::CLI::handleInput(const std::string &input) {
    if (input == _exitCommand) {
        handleExit();
    } else if (input == _helpCommand) {
        handleHelp();
    } else if (input == _statusCommand) {
        handleStatus();
    } else if (_reception.processOrder(utils::toLower(input)) == false) {
        std::cout << "Invalid order: " << input << std::endl;
        std::cout << "Type 'help' for more information." << std::endl;
    }
}

void plazza::CLI::handleExit() {
    std::cout << "Bye" << std::endl;
    exit(0);
}

void plazza::CLI::handleHelp() {
    std::cout << _helpMessage << std::endl;
}

void plazza::CLI::handleStatus() {
    _reception.displayStatus();
}

std::string plazza::CLI::getLastInput() const {
    return _lastInput;
}

plazza::CLI::CLIException::CLIException(const std::string &errmsg)
    : _errmsg(errmsg) {}

const char *plazza::CLI::CLIException::what() const noexcept {
    return _errmsg.c_str();
}
