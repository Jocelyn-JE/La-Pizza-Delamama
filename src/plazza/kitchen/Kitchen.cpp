/*
** EPITECH PROJECT, 2025
** Plazza
** File description:
** Kitchen
*/

#include "Kitchen.hpp"

#include <chrono>
#include <iostream>
#include <thread>

namespace plazza {

Kitchen::Kitchen(unsigned int cookingMultiplier, unsigned int cookNb,
    unsigned int restockTime, std::string kitchenName)
    : _cookingMultiplier(cookingMultiplier),
      _cookNb(cookNb),
      _restockTime(restockTime),
      _kitchenName(kitchenName) {
    _cookStates.reserve(_cookNb);
    for (unsigned int i = 0; i < _cookNb; ++i) {
        _cookStates.emplace_back();
    }

    _cookMutexes.reserve(_cookNb);
    for (unsigned int i = 0; i < _cookNb; ++i) {
        _cookMutexes.emplace_back(std::make_unique<std::mutex>());
    }

    for (unsigned int i = 0; i < _cookNb; ++i) {
        startCookThread(i);
    }

    _restockThread = std::thread(&Kitchen::restockWorker, this);

    std::cerr << "Kitchen " << _kitchenName << " initialized with " << _cookNb
              << " cooks" << std::endl;
}

Kitchen::~Kitchen() {
    _running = false;
    _kitchenOpen = false;

    for (auto &thread : _cookThreads) {
        if (thread.joinable()) {
            thread.join();
        }
    }

    if (_restockThread.joinable()) {
        _restockThread.join();
    }

    std::cerr << "Kitchen " << _kitchenName << " shut down" << std::endl;
}

void Kitchen::startCookThread(unsigned int cookId) {
    _cookThreads.emplace_back(&Kitchen::cookWorker, this, cookId);
}

bool Kitchen::assignPizzaToCook(const plazza::Pizza &pizza) {
    for (unsigned int i = 0; i < _cookNb; ++i) {
        std::lock_guard<std::mutex> lock(*_cookMutexes[i]);

        if (_cookStates[i].canAcceptPizza()) {
            if (!_cookStates[i].isCooking) {
                _cookStates[i].currentPizza = pizza;
                _cookStates[i].isCooking = true;
                std::cerr << "Assigned " << pizza.toString() << " to cook "
                          << i << " (cooking)" << std::endl;
            } else {
                _cookStates[i].queuedPizza = pizza;
                _cookStates[i].hasQueued = true;
                std::cerr << "Assigned " << pizza.toString() << " to cook "
                          << i << " (queued)" << std::endl;
            }
            return true;
        }
    }

    std::cerr << "All cooks in kitchen " << _kitchenName << " are full"
              << std::endl;
    return false;
}

void Kitchen::cookWorker(unsigned int cookId) {
    std::cerr << "Cook " << cookId << " in kitchen " << _kitchenName
              << " started" << std::endl;

    while (_running && _kitchenOpen) {
        plazza::Pizza pizzaToCook(
            plazza::Pizza::NONE_TYPE, plazza::Pizza::NONE_SIZE);
        bool hasPizza = false;

        {
            std::lock_guard<std::mutex> lock(*_cookMutexes[cookId]);
            if (_cookStates[cookId].isCooking) {
                pizzaToCook = _cookStates[cookId].currentPizza;
                hasPizza = true;
            }
        }

        if (hasPizza) {
            if (decrementIngredients(pizzaToCook)) {
                unsigned int cookTime =
                    pizzaToCook.getPizzaTime() * _cookingMultiplier;
                std::cerr << "Cook " << cookId << " cooking "
                          << pizzaToCook.toString() << " for " << cookTime
                          << "ms" << std::endl;

                std::this_thread::sleep_for(
                    std::chrono::milliseconds(cookTime));

                std::cerr << "Cook " << cookId << " finished cooking "
                          << pizzaToCook.toString() << std::endl;

                {
                    std::lock_guard<std::mutex> lock(*_cookMutexes[cookId]);
                    if (_cookStates[cookId].hasQueued) {
                        _cookStates[cookId].currentPizza =
                            _cookStates[cookId].queuedPizza;
                        _cookStates[cookId].queuedPizza =
                            plazza::Pizza(plazza::Pizza::NONE_TYPE,
                                plazza::Pizza::NONE_SIZE);
                        _cookStates[cookId].hasQueued = false;
                        // isCooking remains true
                    } else {
                        _cookStates[cookId].isCooking = false;
                        _cookStates[cookId].currentPizza =
                            plazza::Pizza(plazza::Pizza::NONE_TYPE,
                                plazza::Pizza::NONE_SIZE);
                    }
                }
            } else {
                std::cerr << "Cook " << cookId
                          << " waiting for ingredients for "
                          << pizzaToCook.toString() << std::endl;
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
        } else {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }

    std::cerr << "Cook " << cookId << " in kitchen " << _kitchenName
              << " finished" << std::endl;
}

bool Kitchen::decrementIngredients(const plazza::Pizza &pizza) {
    std::lock_guard<std::mutex> lock(_ingredientsMutex);

    switch (pizza.getType()) {
        case plazza::Pizza::Margarita:
            if (_ingredients.dough < 1 || _ingredients.tomato < 1 ||
                _ingredients.gruyere < 1) {
                return false;
            }
            _ingredients.dough -= 1;
            _ingredients.tomato -= 1;
            _ingredients.gruyere -= 1;
            break;

        case plazza::Pizza::Regina:
            if (_ingredients.dough < 1 || _ingredients.tomato < 1 ||
                _ingredients.gruyere < 1 || _ingredients.ham < 1 ||
                _ingredients.mushrooms < 1) {
                return false;
            }
            _ingredients.dough -= 1;
            _ingredients.tomato -= 1;
            _ingredients.gruyere -= 1;
            _ingredients.ham -= 1;
            _ingredients.mushrooms -= 1;
            break;

        case plazza::Pizza::Americana:
            if (_ingredients.dough < 1 || _ingredients.tomato < 1 ||
                _ingredients.gruyere < 1 || _ingredients.steak < 1) {
                return false;
            }
            _ingredients.dough -= 1;
            _ingredients.tomato -= 1;
            _ingredients.gruyere -= 1;
            _ingredients.steak -= 1;
            break;

        case plazza::Pizza::Fantasia:
            if (_ingredients.dough < 1 || _ingredients.tomato < 1 ||
                _ingredients.eggplant < 1 || _ingredients.goatCheese < 1 ||
                _ingredients.chiefLove < 1) {
                return false;
            }
            _ingredients.dough -= 1;
            _ingredients.tomato -= 1;
            _ingredients.eggplant -= 1;
            _ingredients.goatCheese -= 1;
            _ingredients.chiefLove -= 1;
            break;

        default:
            return false;
    }

    return true;
}

unsigned int Kitchen::getCurrentLoad() const {
    unsigned int totalLoad = 0;

    for (unsigned int i = 0; i < _cookNb; ++i) {
        std::lock_guard<std::mutex> lock(*_cookMutexes[i]);
        totalLoad += _cookStates[i].getCurrentLoad();
    }

    return totalLoad;
}

KitchenStatus Kitchen::getCurrentStatus() const {
    KitchenStatus status;

    status.busyCooks = 0;
    status.queueSize = 0;

    for (unsigned int i = 0; i < _cookNb; ++i) {
        std::lock_guard<std::mutex> lock(*_cookMutexes[i]);
        if (_cookStates[i].isCooking) {
            status.busyCooks++;
        }
        status.queueSize += _cookStates[i].getCurrentLoad();
    }

    {
        std::lock_guard<std::mutex> lock(_ingredientsMutex);
        status.ingredients = _ingredients;
    }

    return status;
}

void Kitchen::restockWorker() {
    while (_running && _kitchenOpen) {
        std::this_thread::sleep_for(std::chrono::seconds(_restockTime));

        if (_running && _kitchenOpen) {
            std::lock_guard<std::mutex> lock(_ingredientsMutex);
            _ingredients.restock();
            std::cerr << "Kitchen " << _kitchenName << " restocked ingredients"
                      << std::endl;
        }
    }
}

}  // namespace plazza
