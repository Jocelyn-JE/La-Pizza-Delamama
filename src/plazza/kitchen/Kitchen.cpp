/*
** EPITECH PROJECT, 2025
** Plazza
** File description:
** Kitchen
*/

#include "Kitchen.hpp"

#include <iostream>

static void cookThread(plazza::Kitchen &kitchen) {
    SafeQueue<plazza::Pizza> &pizzasToCook = kitchen.getPizzasToCook();
    SafeQueue<plazza::Pizza> &pizzasCooked = kitchen.getPizzasCooked();
    plazza::Pizza pizza;
    bool hasPizza = false;
    while (kitchen.isOpen()) {
        if (pizzasToCook.tryPop(pizza)) {
            if (kitchen.decrementIngredients(pizza)) {
                hasPizza = true;
            } else {
                std::cout << "Not enough ingredients for pizza: "
                          << pizza.getType() << " of size " << pizza.getSize()
                          << std::endl;
                continue; // Skip cooking if not enough ingredients
            }
            // Simulate cooking time based on pizza size and cooking multiplier
            std::this_thread::sleep_for(std::chrono::milliseconds(
                pizza.getPizzaTime() * kitchen.getCookingMultiplier()));
            pizzasCooked.push(pizza);
            std::cout << "Pizza cooked: " << pizza.getType() << " of size "
                      << pizza.getSize() << std::endl;
        }
        hasPizza = false;
    }
    std::cout << "Cook thread finished for kitchen: " << kitchen.getKitchenName() << std::endl;
}

namespace plazza {
Kitchen::Kitchen(unsigned int cookingMultiplier, unsigned int cookNb,
    unsigned int restockTime, std::string kitchenName)
    : _cookingMultiplier(cookingMultiplier),
      _cookNb(cookNb),
      _restockTime(restockTime),
      _kitchenName(kitchenName),
      _kitchenOpen(true) {}

Kitchen::~Kitchen() {
    for (auto &cook : _cooks) {
        if (cook.joinable()) {
            cook.join();
        }
    }
    std::cout << "Kitchen " << this->_kitchenName << " closed." << std::endl;
    this->_cooks.clear();
    this->_timePassed = 0;
    this->_lastRestockTime = 0;
    this->_lastCookTime = 0;
}

bool Kitchen::decrementIngredients(const plazza::Pizza &pizza) {
    _ingredientsMutex.lock();
    if (!this->_kitchenOpen) {
        _ingredientsMutex.unlock();
        return false; // Kitchen is closed, cannot decrement ingredients
    }
    switch (pizza.getType()) {
        case plazza::Pizza::Margarita:
            if (this->_ingredients.dough < 1 || this->_ingredients.tomato < 1
                || this->_ingredients.gruyere < 1) {
                _ingredientsMutex.unlock();
                return false; // Not enough ingredients
            }
            this->_ingredients.dough -= 1;
            this->_ingredients.tomato -= 1;
            this->_ingredients.gruyere -= 1;
            break;
        case plazza::Pizza::Regina:
            if (this->_ingredients.dough < 1 || this->_ingredients.tomato < 1
                || this->_ingredients.gruyere < 1 || this->_ingredients.ham < 1
                || this->_ingredients.mushrooms < 1) {
                _ingredientsMutex.unlock();
                return false; // Not enough ingredients
            }
            this->_ingredients.dough -= 1;
            this->_ingredients.tomato -= 1;
            this->_ingredients.gruyere -= 1;
            this->_ingredients.ham -= 1;
            this->_ingredients.mushrooms -= 1;
            break;
        case plazza::Pizza::Americana:
            if (this->_ingredients.dough < 1 || this->_ingredients.tomato < 1
                || this->_ingredients.gruyere < 1 || this->_ingredients.steak < 1) {
                _ingredientsMutex.unlock();
                return false; // Not enough ingredients
            }
            this->_ingredients.dough -= 1;
            this->_ingredients.tomato -= 1;
            this->_ingredients.gruyere -= 1;
            this->_ingredients.steak -= 1;
            break;
        case plazza::Pizza::Fantasia:
            if (this->_ingredients.dough < 1 || this->_ingredients.tomato < 1
                || this->_ingredients.gruyere < 1 || this->_ingredients.eggplant < 1
                || this->_ingredients.goatCheese < 1) {
                _ingredientsMutex.unlock();
                return false; // Not enough ingredients
            }
            this->_ingredients.dough -= 1;
            this->_ingredients.tomato -= 1;
            this->_ingredients.gruyere -= 1;
            this->_ingredients.eggplant -= 1;
            this->_ingredients.goatCheese -= 1;
            this->_ingredients.chiefLove -= 1;
            break;
        default:
            _ingredientsMutex.unlock();
            return false; // Invalid pizza type
    }
    _ingredientsMutex.unlock();
    return true; // Ingredients decremented successfully
}

bool Kitchen::isOpen() const {
    return this->_kitchenOpen;
}

// implement the thread pool for cooks
void Kitchen::cook() {
    this->_lastCookTime = std::time(nullptr);
    this->_lastRestockTime = std::time(nullptr);
    std::cout << "Create the " << this->_cookNb
              << " cooks for kitchen: " << this->_kitchenName << std::endl;

    for (unsigned int i = 0; i < this->_cookNb; ++i) {
        this->_cooks.emplace_back(cookThread, std::ref(*this));
        std::cout << "Cook " << i + 1 << " created for kitchen: "
                  << this->_kitchenName << std::endl;
    }

    while (1) {
        std::clock_t currentTime = std::clock();
        this->_timePassed = currentTime;

        if (std::difftime(std::time(nullptr), this->_lastRestockTime) >= this->_restockTime/1000.0f) {
            this->_lastRestockTime = std::time(nullptr);
            this->_ingredients.restock();
            std::cout << "Restocking ingredients in kitchen: "
                      << this->_kitchenName << std::endl;
        }
        if (std::difftime(std::time(nullptr), this->_lastCookTime) >= 5) {
            std::cout << "Closing kitchen: " << this->_kitchenName << std::endl;
            break;
        }
        // if status received, print status
        // if order received, process order and print OK or KO
        // if pizza cooked, print pizza type and size and finished
        // thread pool
    }
}
}