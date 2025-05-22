/*
** EPITECH PROJECT, 2025
** La-Pizza-Delamama
** File description:
** Main
*/

#include "CLI.hpp"
#include "Parser.hpp"

int main(int ac, char **av) {
    Parser parser(ac, av);
    if (!parser.parse())
        return 84;
    plazza::CLI shell(parser.getCookingMultiplier(), parser.getCookNb(),
        parser.getRestockTime());
    shell.runInterface();
    return 0;
}
