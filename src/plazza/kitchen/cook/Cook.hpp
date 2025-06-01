/*
** EPITECH PROJECT, 2025
** La-Pizza-Delamama
** File description:
** Cook
*/

#ifndef SRC_PLAZZA_KITCHEN_COOK_COOK_HPP_
#define SRC_PLAZZA_KITCHEN_COOK_COOK_HPP_

#include <atomic>
#include <chrono>
#include <memory>
#include <mutex>
#include <thread>

#include "../../Pizza.hpp"
#include "../../../SafeQueue.hpp"

namespace plazza {

class Kitchen;

class Cook {
 public:
    struct State {
        bool isCooking;
        bool hasQueued;
        plazza::Pizza currentPizza;
        plazza::Pizza queuedPizza;

        State() : isCooking(false), hasQueued(false),
                 currentPizza(plazza::Pizza::NONE_TYPE, plazza::Pizza::NONE_SIZE),
                 queuedPizza(plazza::Pizza::NONE_TYPE, plazza::Pizza::NONE_SIZE) {}

        bool canAcceptPizza() const {
            return !isCooking || !hasQueued;
        }

        int getCurrentLoad() const {
            int load = 0;
            if (isCooking) load++;
            if (hasQueued) load++;
            return load;
        }
    };

    Cook(Kitchen& kitchen, unsigned int id, std::mutex& mutex);
    ~Cook();

    void start();
    void stop();
    void assignPizza(const plazza::Pizza& pizza);
    State getState() const;
    unsigned int getId() const;

 private:
    void worker();
    bool tryCookPizza(const plazza::Pizza& pizza);

    Kitchen& _kitchen;
    unsigned int _id;
    std::mutex& _mutex;
    State _state;
    std::thread _thread;
    std::atomic<bool> _running;
};

}  // namespace plazza

#endif  // SRC_PLAZZA_KITCHEN_COOK_COOK_HPP_