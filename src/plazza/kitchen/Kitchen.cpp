/*
** EPITECH PROJECT, 2025
** Plazza
** File description:
** Kitchen
*/

#include "Kitchen.hpp"

#include <iostream>
#include <string>

namespace plazza {
Kitchen::Kitchen(unsigned int cookingMultiplier, unsigned int cookNb,
    unsigned int restockTime, std::string kitchenName)
    : _cookingMultiplier(cookingMultiplier),
      _cookNb(cookNb),
      _restockTime(restockTime),
      _kitchenName(kitchenName) {
    _cooks.reserve(cookNb);
}

Kitchen::~Kitchen() {
    for (auto &cook : _cooks) {
        if (cook.joinable()) {
            cook.join();
        }
    }
    std::cout << "Kitchen " << this->_kitchenName << " closed." << std::endl;
    this->_cooks.clear();
    this->_pizzasToCook.clear();
    this->_timePassed = 0;
    this->_lastRestockTime = 0;
    this->_lastCookTime = 0;
}

// implement the thread pool for cooks
void Kitchen::cook() {
    this->_lastCookTime = std::time(nullptr);
    this->_lastRestockTime = std::time(nullptr);
    std::cout << "Create the " << this->_cookNb
              << " cooks for kitchen: " << this->_kitchenName << std::endl;

    while (1) {
        std::clock_t currentTime = std::clock();
        this->_timePassed = currentTime;

        if (std::difftime(std::time(nullptr), this->_lastRestockTime) >=
            this->_restockTime / 1000.0f) {
            this->_lastRestockTime = std::time(nullptr);
            this->_ingredients.restock();
            std::cout << "Restocking ingredients in kitchen: "
                      << this->_kitchenName << std::endl;
        }
        if (std::difftime(std::time(nullptr), this->_lastCookTime) >= 5) {
            std::cout << "Closing kitchen: " << this->_kitchenName
                      << std::endl;
            break;
        }
        // if status received, print status
        // if order received, process order and print OK or KO
        // if pizza cooked, print pizza type and size and finished
        // thread pool
    }
}
}  // namespace plazza
