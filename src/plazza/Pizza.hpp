/*
** EPITECH PROJECT, 2025
** La-Pizza-Delamama
** File description:
** Pizza
*/

#ifndef SRC_PLAZZA_PIZZA_HPP_
#define SRC_PLAZZA_PIZZA_HPP_

namespace plazza {
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

    Pizza() = delete;
    Pizza(PizzaType type, PizzaSize size);
    ~Pizza() = default;

 private:
    const PizzaType _type;
    const PizzaSize _size;
};
}  // namespace plazza

#endif  // SRC_PLAZZA_PIZZA_HPP_
