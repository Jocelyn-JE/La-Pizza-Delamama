/*
** EPITECH PROJECT, 2025
** La-Pizza-Delamama
** File description:
** Reception
*/

#ifndef SRC_PLAZZA_RECEPTION_RECEPTION_HPP_
#define SRC_PLAZZA_RECEPTION_RECEPTION_HPP_
#include <string>

namespace plazza {
class Reception {
 public:
    Reception() = delete;
    Reception(double cookingMultiplier, unsigned int cookNb,
        unsigned int restockTime);
    ~Reception() = default;
    // Function to process an order, returns true if the order is valid
    bool processOrder(const std::string &order);
    void createNewKitchen();

 protected:
 private:
    double _cookingMultiplier;
    unsigned int _cookNb;
    unsigned int _restockTime;
    bool validatePizza(const std::string &pizza);
};
}  // namespace plazza

#endif  // SRC_PLAZZA_RECEPTION_RECEPTION_HPP_
