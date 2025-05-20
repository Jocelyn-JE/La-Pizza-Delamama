/*
** EPITECH PROJECT, 2025
** La-Pizza-Delamama
** File description:
** Reception
*/

#include "Reception.hpp"

#include <iostream>
#include <string>
#include <vector>

plazza::Reception::Reception(
    double cookingMultiplier, unsigned int cookNb, unsigned int restockTime)
    : _cookingMultiplier(cookingMultiplier),
      _cookNb(cookNb),
      _restockTime(restockTime) {}

static std::vector<std::string> split(
    const std::string &str, const std::string &delimiter) {
    std::vector<std::string> tokens;
    size_t start = 0, end = 0;

    end = str.find(delimiter, start);
    while (end != std::string::npos) {
        tokens.push_back(str.substr(start, end - start));
        start = end + delimiter.length();
        end = str.find(delimiter, start);
    }
    if (start < str.length())
        tokens.push_back(str.substr(start));
    return tokens;
}

bool plazza::Reception::processOrder(const std::string &order) {
    std::vector<std::string> pizzaOrders = split(order, "; ");

    for (auto pizza : pizzaOrders) {
        // std::cout << "Pizza order:" << pizza << std::endl;
        if (!validatePizza(pizza)) {
            std::cout << "Invalid pizza order: " << pizza << std::endl;
            return false;
        }
    }
    return true;
}

bool plazza::Reception::validatePizza(const std::string &pizza) {
    std::string validPizzas[] = {"regina", "margarita", "americana",
        "fantasia"};  // Probably gonna be replaced by a global constant
    std::vector<std::string> tokenizedPizza = split(pizza, " ");

    // for (auto token : tokenizedPizza) {
    //     std::cout << token << std::endl;
    // }
    if (tokenizedPizza.size() != 3) {
        std::cout << "Missing info: " << pizza << std::endl;
        return false;
    }
    return true;
}
