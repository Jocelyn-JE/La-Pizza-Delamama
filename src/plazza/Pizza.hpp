/*
** EPITECH PROJECT, 2025
** La-Pizza-Delamama
** File description:
** Pizza
*/

#ifndef SRC_PLAZZA_PIZZA_HPP_
#define SRC_PLAZZA_PIZZA_HPP_
#include <string>
#include <unordered_map>
#include <unordered_set>

namespace plazza {
static const std::unordered_set<std::string> validPizzaTypes = {
    "regina", "margarita", "americana", "fantasia"};
static const std::unordered_set<std::string> validPizzaSizes = {
    "S", "M", "L", "XL", "XXL"};

class Pizza {
 public:
    enum PizzaType {
        Regina = 1,
        Margarita = 2,
        Americana = 4,
        Fantasia = 8
    };

    enum PizzaSize {
        S = 1,
        M = 2,
        L = 4,
        XL = 8,
        XXL = 16
    };

    Pizza() = default;
    Pizza(PizzaType type, PizzaSize size);
    ~Pizza() = default;

    PizzaType getType() const {
        return _type;
    }

    PizzaSize getSize() const {
        return _size;
    }

    std::string toString() const;

 private:
    PizzaType _type;
    PizzaSize _size;
};

}  // namespace plazza

#endif  // SRC_PLAZZA_PIZZA_HPP_
