/*
** EPITECH PROJECT, 2025
** La-Pizza-Delamama
** File description:
** Pizza
*/

#include "Pizza.hpp"

plazza::Pizza::Pizza(PizzaType type, PizzaSize size)
    : _type(type), _size(size) {}

plazza::Pizza::Pizza()
    : _type(PizzaType::NONE_TYPE), _size(PizzaSize::NONE_SIZE) {}

plazza::Pizza::PizzaType plazza::Pizza::getType() {
    return _type;
}

plazza::Pizza::PizzaSize plazza::Pizza::getSize() {
    return _size;
}

std::time_t plazza::Pizza::getPizzaTime() const {
    if (_type == PizzaType::NONE_TYPE || _size == PizzaSize::NONE_SIZE) {
        return 0;  // Invalid pizza
    }
    switch (_type) {
        case PizzaType::Regina:
            return 1000;  // Example time for Regina
        case PizzaType::Margarita:
            return 2000;  // Example time for Margarita
        case PizzaType::Americana:
            return 2000;  // Example time for Americana
        case PizzaType::Fantasia:
            return 4000;  // Example time for Fantasia
        default:
            return 0;  // Should not happen, but safe fallback
    }
}
