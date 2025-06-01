/*
** EPITECH PROJECT, 2025
** La-Pizza-Delamama
** File description:
** Cook
*/

#ifndef SRC_PLAZZA_KITCHEN_COOK_COOK_HPP_
#define SRC_PLAZZA_KITCHEN_COOK_COOK_HPP_

#include <chrono>
#include <thread>

#include "../../Pizza.hpp"

namespace plazza {

class Kitchen;

class Cook {
 public:
    plazza::Kitchen &kitchen;
    bool working;
    plazza::Pizza pizza;

    explicit Cook(plazza::Kitchen &kitchen);
    void cook();
    void dump();
};
}  // namespace plazza

#endif  // SRC_PLAZZA_KITCHEN_COOK_COOK_HPP_
