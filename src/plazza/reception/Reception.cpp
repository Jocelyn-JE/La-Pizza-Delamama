/*
** EPITECH PROJECT, 2025
** La-Pizza-Delamama
** File description:
** Reception
*/

#include "Reception.hpp"
#include <unistd.h>

#include <cstdio>
#include <exception>
#include <iostream>
#include <string>
#include <vector>

#include "Utils.hpp"
#include "plazza/Pizza.hpp"
#include "plazza/kitchen/Kitchen.hpp"

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
        sendPizzaToKitchen(pizza);
    }
    return true;
}

void plazza::Reception::sendPizzaToKitchen(const std::string &pizza) {
    if (_kitchenPipes.empty())
        createNewKitchen();
    std::cout << "Sending pizza to kitchen: "
              << _kitchenPipes.back().getPipePath() << std::endl;
    _kitchenPipes.back().writeString(pizza);
    std::cout << "Sent pizza to kitchen: " << pizza << std::endl;
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
    pid_t pid;

    _kitchenPipes.emplace_back(
        "/tmp/pizza_pipe_" + std::to_string(_kitchenCount));
    _kitchenCount++;
    pid = fork();
    if (pid == -1) {
        perror("fork");
        throw std::runtime_error("Failed to fork process");
    }
    if (pid == 0) {
        try {
            std::cout << "Child process created for kitchen: "
                      << _kitchenPipes.back().getPipePath() << std::endl;
            Kitchen kitchen(_cookingMultiplier, _cookNb, _restockTime,
                _kitchenPipes.back());
            kitchen.cook();
        } catch (std::exception &e) {
            std::cout << "Child process error: " << e.what() << std::endl;
        }
        exit(0);
    } else {
        std::cout << "Parent: " << _kitchenPipes.back().getPipePath()
                  << std::endl;
    }
}
