/*
** EPITECH PROJECT, 2025
** La-Pizza-Delamama
** File description:
** Pizza
*/

#include "Pizza.hpp"

#include <sstream>

plazza::Pizza::Pizza(PizzaType type, PizzaSize size)
    : _type(type), _size(size) {}

std::string plazza::Pizza::toString() const {
    std::stringstream ss;

    switch (_type) {
        case PizzaType::Regina:
            ss << "Regina";
            break;
        case PizzaType::Margarita:
            ss << "Margarita";
            break;
        case PizzaType::Americana:
            ss << "Americana";
            break;
        case PizzaType::Fantasia:
            ss << "Fantasia";
            break;
    }

    ss << " ";

    switch (_size) {
        case PizzaSize::S:
            ss << "S";
            break;
        case PizzaSize::M:
            ss << "M";
            break;
        case PizzaSize::L:
            ss << "L";
            break;
        case PizzaSize::XL:
            ss << "XL";
            break;
        case PizzaSize::XXL:
            ss << "XXL";
            break;
    }

    return ss.str();
}
