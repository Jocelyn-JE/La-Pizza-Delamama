/*
** EPITECH PROJECT, 2025
** La-Pizza-Delamama
** File description:
** CLI
*/

#include "CLI.hpp"

#include <iostream>
#include <string>

plazza::CLI::CLI() {}

plazza::CLI::~CLI() {}

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
    } else {
        // Won't be unknown command but this is where we'll handle pizza orders
        std::cout << "Unknown command: " << input << std::endl;
    }
}

void plazza::CLI::handleExit() {
    std::cout << "Bye" << std::endl;
    exit(0);
}

void plazza::CLI::handleHelp() {
    std::cout << _helpMessage << std::endl;
}

std::string plazza::CLI::getLastInput() const {
    return _lastInput;
}

// CLIException

plazza::CLI::CLIException::CLIException(const std::string &errmsg)
    : _errmsg(errmsg) {}

const char *plazza::CLI::CLIException::what() const noexcept {
    return _errmsg.c_str();
}
