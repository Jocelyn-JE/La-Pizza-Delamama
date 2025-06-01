#include "Kitchen.hpp"

#include <chrono>
#include <iostream>
#include <thread>

#include "cook/Cook.hpp"

namespace plazza {

Kitchen::Kitchen(double cookingMultiplier, unsigned int cookNb,
    unsigned int restockTime, std::string kitchenName)
    : _cookingMultiplier(cookingMultiplier),
      _cookNb(cookNb),
      _restockTime(restockTime),
      _kitchenName(kitchenName) {
    _cookMutexes.reserve(_cookNb);
    _cooks.reserve(_cookNb);

    for (unsigned int i = 0; i < _cookNb; ++i) {
        _cookMutexes.emplace_back(std::make_unique<std::mutex>());
        _cooks.emplace_back(
            std::make_unique<Cook>(*this, i, *_cookMutexes.back()));
        _cooks.back()->start();
    }

    _restockThread = std::thread(&Kitchen::restockWorker, this);

    std::cerr << "Kitchen " << _kitchenName << " initialized with " << _cookNb
              << " cooks" << std::endl;
}

Kitchen::~Kitchen() {
    _running = false;
    _kitchenOpen = false;

    for (auto &cook : _cooks) {
        cook->stop();
    }

    if (_restockThread.joinable()) {
        _restockThread.join();
    }

    std::cerr << "Kitchen " << _kitchenName << " shut down" << std::endl;
}

bool Kitchen::assignPizzaToCook(const plazza::Pizza &pizza) {
    for (auto &cook : _cooks) {
        if (cook->tryAssignPizza(pizza)) {
            return true;
        }
    }
    return false;
}

unsigned int Kitchen::getCurrentLoad() const {
    unsigned int totalLoad = 0;
    for (const auto &cook : _cooks) {
        totalLoad += cook->getState().getCurrentLoad();
    }
    return totalLoad;
}

KitchenStatus Kitchen::getCurrentStatus() const {
    KitchenStatus status;
    status.busyCooks = 0;
    status.queueSize = 0;

    for (const auto &cook : _cooks) {
        auto cookState = cook->getState();
        if (cookState.isCooking) {
            status.busyCooks++;
        }
        status.queueSize += cookState.getCurrentLoad();
    }

    {
        std::lock_guard<std::mutex> lock(_ingredientsMutex);
        status.ingredients = _ingredients;
    }

    return status;
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

void Kitchen::restockWorker() {
    while (_running && _kitchenOpen) {
        std::this_thread::sleep_for(std::chrono::milliseconds(_restockTime));

        if (_running && _kitchenOpen) {
            std::lock_guard<std::mutex> lock(_ingredientsMutex);
            _ingredients.restock();
            std::cerr << "Kitchen " << _kitchenName << " restocked ingredients"
                      << std::endl;
        }
    }
}

}  // namespace plazza
