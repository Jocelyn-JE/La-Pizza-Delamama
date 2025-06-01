/*
** EPITECH PROJECT, 2025
** La-Pizza-Delamama
** File description:
** Cook
*/

#include "Cook.hpp"

#include <iostream>

#include "../Kitchen.hpp"

namespace plazza {

Cook::Cook(Kitchen &kitchen, unsigned int id, std::mutex &mutex)
    : _kitchen(kitchen), _id(id), _mutex(mutex), _running(false) {}

Cook::~Cook() {
    stop();
}

void Cook::start() {
    _running = true;
    _thread = std::thread(&Cook::worker, this);
}

void Cook::stop() {
    _running = false;
    if (_thread.joinable()) {
        _thread.join();
    }
}

bool Cook::tryAssignPizza(const plazza::Pizza &pizza) {
    std::lock_guard<std::mutex> lock(_mutex);

    if (!_state.canAcceptPizza()) {
        return false;
    }

    if (!_state.isCooking) {
        _state.currentPizza = pizza;
        _state.isCooking = true;
    } else {
        _state.queuedPizza = pizza;
        _state.hasQueued = true;
    }

    return true;
}

void Cook::assignPizza(const plazza::Pizza &pizza) {
    std::lock_guard<std::mutex> lock(_mutex);
    if (!_state.isCooking) {
        _state.currentPizza = pizza;
        _state.isCooking = true;
    } else {
        _state.queuedPizza = pizza;
        _state.hasQueued = true;
    }
}

Cook::State Cook::getState() const {
    std::lock_guard<std::mutex> lock(_mutex);
    return _state;
}

unsigned int Cook::getId() const {
    return _id;
}

void Cook::worker() {
    while (_running && _kitchen.isOpen()) {
        plazza::Pizza pizzaToCook(
            plazza::Pizza::NONE_TYPE, plazza::Pizza::NONE_SIZE);
        bool hasPizza = false;

        {
            std::lock_guard<std::mutex> lock(_mutex);
            if (_state.isCooking) {
                pizzaToCook = _state.currentPizza;
                hasPizza = true;
            }
        }

        if (hasPizza) {
            if (_kitchen.decrementIngredients(pizzaToCook)) {
                unsigned int cookTime = pizzaToCook.getPizzaTime() *
                                        _kitchen.getCookingMultiplier();

                std::cerr << "Cook " << _id << " cooking "
                          << pizzaToCook.toString() << " for " << cookTime
                          << "ms" << std::endl;

                std::this_thread::sleep_for(
                    std::chrono::milliseconds(cookTime));

                std::cerr << "Cook " << _id << " finished cooking "
                          << pizzaToCook.toString() << std::endl;

                {
                    std::lock_guard<std::mutex> lock(_mutex);
                    if (_state.hasQueued) {
                        _state.currentPizza = _state.queuedPizza;
                        _state.queuedPizza =
                            plazza::Pizza(plazza::Pizza::NONE_TYPE,
                                plazza::Pizza::NONE_SIZE);
                        _state.hasQueued = false;
                    } else {
                        _state.isCooking = false;
                        _state.currentPizza =
                            plazza::Pizza(plazza::Pizza::NONE_TYPE,
                                plazza::Pizza::NONE_SIZE);
                    }
                }
            } else {
                std::cerr << "Cook " << _id << " waiting for ingredients for "
                          << pizzaToCook.toString() << std::endl;
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
        } else {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
}
}  // namespace plazza
