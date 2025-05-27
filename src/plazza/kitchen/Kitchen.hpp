/*
** EPITECH PROJECT, 2025
** Plazza
** File description:
** Kitchen
*/

#ifndef KITCHEN_HPP_
#define KITCHEN_HPP_

#include <ctime>
#include <string>
#include <thread>
#include <vector>

#include "plazza/Pizza.hpp"

namespace plazza {

class Ingredients {
 public:

    Ingredients() = default;
    ~Ingredients() = default;

    void restock() {
        dough += 1;
        tomato += 1;
        gruyere += 1;
        ham += 1;
        mushrooms += 1;
        steak += 1;
        eggplant += 1;
        goatCheese += 1;
        chiefLove += 1;
    }

    uint8_t dough = 0;
    uint8_t tomato = 0;
    uint8_t gruyere = 0;
    uint8_t ham = 0;
    uint8_t mushrooms = 0;
    uint8_t steak = 0;
    uint8_t eggplant = 0;
    uint8_t goatCheese = 0;
    uint8_t chiefLove = 0;
};

class Kitchen {
 public:
    Kitchen(unsigned int cookingMultiplier, unsigned int cookNb,
        unsigned int restockTime, std::string kitchenName);
    ~Kitchen();
    void cook();

 private:
    unsigned int _cookingMultiplier;
    unsigned int _cookNb;
    unsigned int _restockTime;
    std::string _kitchenName;

    std::clock_t _timePassed;
    std::time_t _lastRestockTime;
    std::time_t _lastCookTime;

    std::vector<std::thread> _cooks;
    std::vector<plazza::Pizza> _pizzasToCook;

    Ingredients _ingredients;
};
}  // namespace plazza

#endif /* !KITCHEN_HPP_ */
