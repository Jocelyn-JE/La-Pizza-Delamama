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

namespace plazza {
static const std::unordered_set<std::string> validPizzaTypes = {
    "regina", "margarita", "americana", "fantasia"};
static const std::unordered_set<std::string> validPizzaSizes = {
    "S", "M", "L", "XL", "XXL"};

class Pizza {
 public:
    enum PizzaType {
        NONE = 0,
        Regina = 1,
        Margarita = 2,
        Americana = 4,
        Fantasia = 8
    };

    enum PizzaSize {
        NONE = 0,
        S = 1,
        M = 2,
        L = 4,
        XL = 8,
        XXL = 16
    };
    std::time_t getPizzaTime() const {
        if (_type == PizzaType::NONE || _size == PizzaSize::NONE) {
            return 0;  // Invalid pizza
        }
        if (_type == PizzaType::Regina) {
            return 1000;  // Example time for Regina
        } else if (_type == PizzaType::Margarita) {
            return 2000;  // Example time for Margarita
        } else if (_type == PizzaType::Americana) {
            return 2000;  // Example time for Americana
        } else if (_type == PizzaType::Fantasia) {
            return 4000;  // Example time for Fantasia
        }
    }

    Pizza() {
        _type = PizzaType::NONE;      // Default type
        _size = PizzaSize::NONE;      // Default size
    }
    Pizza(PizzaType type, PizzaSize size);
    ~Pizza() = default;
    PizzaType getType() const {
        return _type;
    }
    PizzaSize getSize() const {
        return _size;
    }

 private:
    PizzaType _type;
    PizzaSize _size;
};
}  // namespace plazza

#endif  // SRC_PLAZZA_PIZZA_HPP_
