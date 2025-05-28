/*
** EPITECH PROJECT, 2025
** La-Pizza-Delamama
** File description:
** Reception
*/

#include "Reception.hpp"
#include <unistd.h>

#include <cstdio>
#include <iostream>
#include <string>
#include <vector>

#include "NamedPipe.hpp"
#include "Utils.hpp"
#include "plazza/Pizza.hpp"

plazza::Reception::Reception(
    double cookingMultiplier, unsigned int cookNb, unsigned int restockTime)
    : _cookingMultiplier(cookingMultiplier),
      _cookNb(cookNb),
      _restockTime(restockTime) {}

bool plazza::Reception::processOrder(const std::string &order) {
    std::vector<std::string> pizzaOrders =
        utils::split(order, "; ");  // TODO(Samuel) SPLIT AND TRIM

    for (auto pizza : pizzaOrders) {
        if (pizza.empty()) {
            std::cout << "Empty pizza order" << std::endl;
            return false;
        }
        if (!validatePizza(pizza))
            return false;
    }
    return true;
}

bool plazza::Reception::validatePizza(const std::string &pizza) {
    std::vector<std::string> tokenizedPizza = utils::split(pizza, " ");

    if (tokenizedPizza.size() != 3) {
        std::cout << (tokenizedPizza.size() < 3 ? "Too few" : "Too many")
                  << " values: " << pizza << std::endl;
        std::cout << "Expected format: <type> <size> <count>" << std::endl;
        return false;
    }
    if (!utils::isValidPizzaType(tokenizedPizza[0])) {
        std::cout << "Invalid pizza type: " << tokenizedPizza[0] << std::endl;
        std::cout << "Valid types are: ";
        for (const auto &type : plazza::validPizzaTypes)
            std::cout << type << " ";
        std::cout << std::endl;
        return false;
    }
    if (!utils::isValidPizzaSize(tokenizedPizza[1])) {
        std::cout << "Invalid pizza size: " << tokenizedPizza[1] << std::endl;
        std::cout << "Valid sizes are: ";
        for (const auto &size : plazza::validPizzaSizes)
            std::cout << size << " ";
        std::cout << std::endl;
        return false;
    }
    if (!utils::isValidPizzaCount(tokenizedPizza[2])) {
        std::cout << "Invalid pizza count: " << tokenizedPizza[2] << std::endl;
        std::cout
            << "Count should be in the format 'xN' where N is a positive "
               "integer"
            << std::endl;
        return false;
    }
    return true;
}

void plazza::Reception::createNewKitchen() {
    NamedPipe pipe("/tmp/pizza_pipe");
    pid_t pid = fork();

    if (pid == -1) {
        perror("fork");
        throw std::runtime_error("Failed to fork process");
    }
    if (pid == 0) {
        std::cout << "Child: " << pipe.getPipePath() << std::endl;
        while (true) {
            try {
                std::string data = pipe.readString();
                std::cout << "Child received: " << data << std::endl;
                pipe.writeString("Hello back from child! ");
                pipe.writeString("This is a response string. ");
                pipe.writeString("end of response string is here\n");
            } catch (const std::runtime_error &e) {
                std::cerr << "Error in child: " << e.what() << std::endl;
                break;
            }
        }
        exit(0);
    } else {
        std::cout << "Parent: " << pipe.getPipePath() << std::endl;
        while (true) {
            try {
                pipe.writeString("Hello from parent! ");
                pipe.writeString("This is a test string. ");
                pipe.writeString("end of string is here\n");
                std::string response = pipe.readString();
                std::cout << "Parent received: " << response << std::endl;
                sleep(1);
            } catch (const std::runtime_error &e) {
                std::cerr << "Error in parent: " << e.what() << std::endl;
                break;
            }
        }
    }
}
