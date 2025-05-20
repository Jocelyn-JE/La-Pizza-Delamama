/*
** EPITECH PROJECT, 2025
** La-Pizza-Delamama
** File description:
** Main
*/

#include "CLI.hpp"

int main(int ac, char **av) {
    plazza::CLI shell(1.0, 4, 5);

    shell.runInterface();
    (void)ac;
    (void)av;
    return 0;
}
