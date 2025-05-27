/*
** EPITECH PROJECT, 2025
** La-Pizza-Delamama
** File description:
** Main
*/

#include <unistd.h>

#include <cstdio>
#include <iostream>

#include "CLI.hpp"
#include "NamedPipe.hpp"
#include "Parser.hpp"

int main(int ac, char **av) {
    // Parser parser(ac, av);
    // if (!parser.parse())
    //     return 84;
    // plazza::CLI shell(parser.getCookingMultiplier(), parser.getCookNb(),
    //     parser.getRestockTime());
    // shell.runInterface();
    (void)ac;
    (void)av;
    NamedPipe pipe("/tmp/pizza_pipe");
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        return 84;
    }
    if (pid == 0) {
        // Child process
        std::cout << "Child: " << pipe.getPipePath() << std::endl;
    } else {
        // Parent process
        std::cout << "Parent: " << pipe.getPipePath() << std::endl;
    }
    return 0;
}
