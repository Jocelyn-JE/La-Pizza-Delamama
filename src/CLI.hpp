/*
** EPITECH PROJECT, 2025
** La-Pizza-Delamama
** File description:
** CLI
*/

#ifndef SRC_CLI_HPP_
#define SRC_CLI_HPP_
#include <string>

#include "plazza/reception/Reception.hpp"

namespace plazza {
class CLI {
 public:
    CLI() = delete;
    CLI(double cookingMultiplier, unsigned int cookNb,
        unsigned int restockTime);
    ~CLI() = default;
    void runInterface();
    std::string getLastInput() const;

    class CLIException : public std::exception {
     public:
        explicit CLIException(const std::string &errmsg);
        const char *what() const noexcept override;

     private:
        std::string _errmsg = "No error message specified";
    };

 private:
    plazza::Reception _reception;
    std::string _lastInput = "";
    std::string _prompt = "plazza> ";
    std::string _exitCommand = "exit";
    std::string _helpCommand = "help";
    std::string _helpMessage =
        "Available commands:\n"
        "\t- exit: Exit the program\n"
        "\t- help: Show this help message\n"
        "\t- status: Show the status of the kitchen\n";
    void printPrompt();
    void handleInput(const std::string &input);
    void handleExit();
    void handleHelp();
};
};  // namespace plazza

#endif  // SRC_CLI_HPP_
