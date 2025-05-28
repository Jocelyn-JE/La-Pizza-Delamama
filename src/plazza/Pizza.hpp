/*
** EPITECH PROJECT, 2025
** La-Pizza-Delamama
** File description:
** Pizza
*/

#ifndef SRC_PLAZZA_PIZZA_HPP_
#define SRC_PLAZZA_PIZZA_HPP_
#include <ctime>
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

    std::string getTypeString() const;
    std::string getSizeString() const;
    std::time_t getPizzaTime() const;

    Pizza();
    Pizza(PizzaType type, PizzaSize size);
    ~Pizza() = default;
    PizzaType getType() const;
    PizzaSize getSize() const;

    std::string toString() const {
        std::string typeStr, sizeStr;

        switch (_type) {
            case Regina:
                typeStr = "Regina";
                break;
            case Margarita:
                typeStr = "Margarita";
                break;
            case Americana:
                typeStr = "Americana";
                break;
            case Fantasia:
                typeStr = "Fantasia";
                break;
            default:
                typeStr = "Unknown";
                break;
        }

        switch (_size) {
            case S:
                sizeStr = "S";
                break;
            case M:
                sizeStr = "M";
                break;
            case L:
                sizeStr = "L";
                break;
            case XL:
                sizeStr = "XL";
                break;
            case XXL:
                sizeStr = "XXL";
                break;
            default:
                sizeStr = "Unknown";
                break;
        }

        return typeStr + " " + sizeStr;
    }

 private:
    PizzaType _type;
    PizzaSize _size;
};
}  // namespace plazza

#endif  // SRC_PLAZZA_PIZZA_HPP_
