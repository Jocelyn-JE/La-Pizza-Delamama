/*
** EPITECH PROJECT, 2025
** La-Pizza-Delamama
** File description:
** Pizza
*/

#ifndef SRC_PLAZZA_PIZZA_HPP_
#define SRC_PLAZZA_PIZZA_HPP_
#include <string>
#include <unordered_set>
#include <ctime>

namespace plazza {
static const std::unordered_set<std::string> validPizzaTypes = {
    "regina", "margarita", "americana", "fantasia"};
static const std::unordered_set<std::string> validPizzaSizes = {
    "S", "M", "L", "XL", "XXL"};

class Pizza {
 public:
    enum PizzaType {
        NONE_TYPE = 0,
        Regina = 1,
        Margarita = 2,
        Americana = 4,
        Fantasia = 8
    };

    enum PizzaSize {
        NONE_SIZE = 0,
        S = 1,
        M = 2,
        L = 4,
        XL = 8,
        XXL = 16
    };
    std::time_t getPizzaTime() const;

    Pizza();
    Pizza(PizzaType type, PizzaSize size);
    ~Pizza() = default;
    PizzaType getType();
    PizzaSize getSize();

 private:
    PizzaType _type;
    PizzaSize _size;
};
}  // namespace plazza

#endif  // SRC_PLAZZA_PIZZA_HPP_
