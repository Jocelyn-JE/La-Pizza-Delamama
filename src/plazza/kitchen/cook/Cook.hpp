/*
** EPITECH PROJECT, 2025
** La-Pizza-Delamama
** File description:
** Cook
*/

#ifndef COOK_HPP_
#define COOK_HPP_

#include "../../Pizza.hpp"

namespace plazza {

class Kitchen;

class Cook {
    public:
        plazza::Kitchen &kitchen;
        bool working;
        plazza::Pizza pizza;

        Cook(plazza::Kitchen &kitchen);
        void cook();
        void dump();
};
}  // namespace plazza

#endif /* !COOK_HPP_ */
