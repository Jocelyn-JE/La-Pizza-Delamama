/*
** EPITECH PROJECT, 2025
** La-Pizza-Delamama
** File description:
** Pizza
*/

#include "Pizza.hpp"

#include <string>

plazza::Pizza::Pizza(PizzaType type, PizzaSize size)
    : _type(type), _size(size) {}

plazza::Pizza::Pizza()
    : _type(PizzaType::NONE_TYPE), _size(PizzaSize::NONE_SIZE) {}

plazza::Pizza::PizzaType plazza::Pizza::getType() const {
    return _type;
}

plazza::Pizza::PizzaSize plazza::Pizza::getSize() const {
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

std::string plazza::Pizza::getTypeString() const {
    switch (_type) {
        case PizzaType::Regina:
            return "regina";
        case PizzaType::Margarita:
            return "margarita";
        case PizzaType::Americana:
            return "americana";
        case PizzaType::Fantasia:
            return "fantasia";
        default:
            return "unknown";
    }
}

std::string plazza::Pizza::getSizeString() const {
    switch (_size) {
        case PizzaSize::S:
            return "S";
        case PizzaSize::M:
            return "M";
        case PizzaSize::L:
            return "L";
        case PizzaSize::XL:
            return "XL";
        case PizzaSize::XXL:
            return "XXL";
        default:
            return "unknown";
    }
}
