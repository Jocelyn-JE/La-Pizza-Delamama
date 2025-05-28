/*
** EPITECH PROJECT, 2025
** La-Pizza-Delamama
** File description:
** Cook
*/

#include "Cook.hpp"
#include "../src/SafeQueue.hpp"
#include <iostream>
#include "../Kitchen.hpp"

namespace plazza {

    Cook::Cook(plazza::Kitchen &kitchen)
    : kitchen(kitchen), working(false), pizza(plazza::Pizza::NONE_TYPE, plazza::Pizza::NONE_SIZE)
    {
    }

    void Cook::cook()
    {
        SafeQueue<plazza::Pizza> &pizzasToCook = kitchen.getPizzasToCook();
        SafeQueue<plazza::Pizza> &pizzasCooked = kitchen.getPizzasCooked();
        plazza::Pizza pizza;
        working = false;
        while (kitchen.isOpen()) {
            if (pizzasToCook.tryPop(pizza)) {
                if (kitchen.decrementIngredients(pizza)) {
                    working = true;
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
            working = false;
        }
        std::cout << "Cook thread finished for kitchen: " << kitchen.getKitchenName() << std::endl;
    }

    void Cook::dump()
    {
        if (working) {
            std::cout << "Cook is currently working on pizza: "
                      << pizza.getType() << " of size " << pizza.getSize() << std::endl;
        } else {
            std::cout << "Cook is waiting." << std::endl;
        }
    }

} // namespace plazza