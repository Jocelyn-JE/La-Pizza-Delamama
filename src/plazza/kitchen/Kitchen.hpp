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

#include "../Pizza.hpp"
#include "../src/SafeQueue.hpp"

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

    uint16_t dough = 0;
    uint16_t tomato = 0;
    uint16_t gruyere = 0;
    uint16_t ham = 0;
    uint16_t mushrooms = 0;
    uint16_t steak = 0;
    uint16_t eggplant = 0;
    uint16_t goatCheese = 0;
    uint16_t chiefLove = 0;
};

class Kitchen {
 public:
    Kitchen(unsigned int cookingMultiplier, unsigned int cookNb,
        unsigned int restockTime, std::string kitchenName);
    ~Kitchen();
    void cook();
    bool isOpen() const;
    bool decrementIngredients(const plazza::Pizza &pizza);
    SafeQueue<plazza::Pizza> &getPizzasToCook() {
        return _pizzasToCook;
    }
    SafeQueue<plazza::Pizza> &getPizzasCooked() {
        return _pizzasCooked;
    }
    unsigned int getCookingMultiplier() const {
        return _cookingMultiplier;
    }
    unsigned int getCookNb() const {
        return _cookNb;
    }
    unsigned int getRestockTime() const {
        return _restockTime;
    }
    std::string getKitchenName() const {
        return _kitchenName;
    }

 private:
    unsigned int _cookingMultiplier;
    unsigned int _cookNb;
    unsigned int _restockTime;
    std::string _kitchenName;

    std::clock_t _timePassed;
    std::time_t _lastRestockTime;
    std::time_t _lastCookTime;

    std::vector<std::thread> _cooks;
    SafeQueue<plazza::Pizza> _pizzasToCook;
    SafeQueue<plazza::Pizza> _pizzasCooked;

    Ingredients _ingredients;
    std::mutex _ingredientsMutex;

    bool _kitchenOpen;
};
}  // namespace plazza

#endif /* !KITCHEN_HPP_ */
