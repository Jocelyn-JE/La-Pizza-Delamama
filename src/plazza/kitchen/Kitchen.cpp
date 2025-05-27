// src/plazza/kitchen/Kitchen.cpp
#include "Kitchen.hpp"

#include <chrono>
#include <iostream>
#include <thread>

namespace plazza {
namespace kitchen {

Kitchen::Kitchen(unsigned int id, unsigned int cookNb,
    unsigned int restockTime, double cookingMultiplier)
    : _id(id),
      _cookingMultiplier(cookingMultiplier),
      _restockTime(restockTime),
      _cooks(cookNb),
      _running(true),
      _hasWork(false),
      _lastWorkTime(std::chrono::steady_clock::now()) {
    _ingredients["dough"] = 5;
    _ingredients["tomato"] = 5;
    _ingredients["gruyere"] = 5;
    _ingredients["ham"] = 5;
    _ingredients["mushrooms"] = 5;
    _ingredients["steak"] = 5;
    _ingredients["eggplant"] = 5;
    _ingredients["goat cheese"] = 5;
    _ingredients["chief love"] = 5;

    std::string inPipeName = "/tmp/kitchen_" + std::to_string(_id) + "_in";
    std::string outPipeName = "/tmp/kitchen_" + std::to_string(_id) + "_out";

    _inPipe =
        new plazza::ipc::NamedPipe(inPipeName, plazza::ipc::NamedPipe::READ);
    _outPipe =
        new plazza::ipc::NamedPipe(outPipeName, plazza::ipc::NamedPipe::WRITE);

    _restockThread = std::thread(&Kitchen::restockIngredients, this);

    _workWatcherThread = std::thread(&Kitchen::watchForWork, this);
}

Kitchen::~Kitchen() {
    _running = false;

    if (_restockThread.joinable()) {
        _restockThread.join();
    }

    if (_workWatcherThread.joinable()) {
        _workWatcherThread.join();
    }

    delete _inPipe;
    delete _outPipe;
}

void Kitchen::run() {
    std::cout << "Kitchen " << _id << " started with "
              << _cooks.getThreadCount() << " cooks" << std::endl;

    while (_running) {
        handleCommand();

        processPizzaQueue();

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    std::cout << "Kitchen " << _id << " closed" << std::endl;
}

void Kitchen::handleCommand() {
    int command;
    if (_inPipe->read(command)) {
        switch (command) {
            case 1: {  // Status request
                KitchenStatus status;
                status.busyCooks = _cooks.getBusyCount();
                status.queueSize = getQueueSize();
                status.ingredients = _ingredients;
                _outPipe->write(status);
                break;
            }
            case 2: {  // Load request
                unsigned int load = getQueueSize() + _cooks.getBusyCount();
                _outPipe->write(load);
                break;
            }
            case 3: {  // Pizza order
                Pizza pizza;
                if (_inPipe->read(pizza)) {
                    addPizza(pizza);
                }
                break;
            }
            default:
                break;
        }
    }
}

bool Kitchen::addPizza(const plazza::Pizza &pizza) {
    std::lock_guard<std::mutex> lock(_queueMutex);

    if (_pizzaQueue.size() + _cooks.getBusyCount() >=
        2 * _cooks.getThreadCount()) {
        return false;
    }

    _pizzaQueue.push(pizza);
    _hasWork = true;
    _lastWorkTime = std::chrono::steady_clock::now();
    _queueCondition.notify_one();

    return true;
}

size_t Kitchen::getQueueSize() const {
    std::lock_guard<std::mutex> lock(_queueMutex);
    return _pizzaQueue.size();
}

size_t Kitchen::getBusyCooks() const {
    return _cooks.getBusyCount();
}

const std::unordered_map<std::string, unsigned int> &Kitchen::getIngredients()
    const {
    return _ingredients;
}

void Kitchen::restockIngredients() {
    while (_running) {
        std::this_thread::sleep_for(std::chrono::milliseconds(_restockTime));

        std::lock_guard<std::mutex> lock(_ingredientsMutex);
        for (auto &ingredient : _ingredients) {
            ingredient.second += 1;
        }
    }
}

void Kitchen::watchForWork() {
    while (_running) {
        std::this_thread::sleep_for(std::chrono::seconds(1));

        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(
            now - _lastWorkTime)
                           .count();

        if (!_hasWork && elapsed > 5) {
            std::cout << "Kitchen " << _id << " is closing due to inactivity."
                      << std::endl;
            _running = false;
            break;
        }

        _hasWork = false;
    }
}

bool Kitchen::cookPizza(const plazza::Pizza &pizza) {
    if (!hasEnoughIngredients(pizza)) {
        return false;
    }

    useIngredients(pizza);

    int cookingTime = 0;
    switch (pizza.getType()) {
        case plazza::Pizza::PizzaType::Margarita:
            cookingTime = 1000;
            break;
        case plazza::Pizza::PizzaType::Regina:
            cookingTime = 2000;
            break;
        case plazza::Pizza::PizzaType::Americana:
            cookingTime = 2000;
            break;
        case plazza::Pizza::PizzaType::Fantasia:
            cookingTime = 4000;
            break;
    }

    cookingTime *= _cookingMultiplier;

    std::this_thread::sleep_for(std::chrono::milliseconds(cookingTime));

    _outPipe->write(pizza);

    return true;
}

bool Kitchen::hasEnoughIngredients(const plazza::Pizza &pizza) {
    std::lock_guard<std::mutex> lock(_ingredientsMutex);

    if (_ingredients["dough"] < 1 || _ingredients["tomato"] < 1) {
        return false;
    }

    switch (pizza.getType()) {
        case plazza::Pizza::PizzaType::Margarita:
            return _ingredients["gruyere"] >= 1;
        case plazza::Pizza::PizzaType::Regina:
            return _ingredients["gruyere"] >= 1 && _ingredients["ham"] >= 1 &&
                   _ingredients["mushrooms"] >= 1;
        case plazza::Pizza::PizzaType::Americana:
            return _ingredients["gruyere"] >= 1 && _ingredients["steak"] >= 1;
        case plazza::Pizza::PizzaType::Fantasia:
            return _ingredients["eggplant"] >= 1 &&
                   _ingredients["goat cheese"] >= 1 &&
                   _ingredients["chief love"] >= 1;
        default:
            return false;
    }
}

void Kitchen::useIngredients(const plazza::Pizza &pizza) {
    std::lock_guard<std::mutex> lock(_ingredientsMutex);

    _ingredients["dough"] -= 1;
    _ingredients["tomato"] -= 1;

    switch (pizza.getType()) {
        case plazza::Pizza::PizzaType::Margarita:
            _ingredients["gruyere"] -= 1;
            break;
        case plazza::Pizza::PizzaType::Regina:
            _ingredients["gruyere"] -= 1;
            _ingredients["ham"] -= 1;
            _ingredients["mushrooms"] -= 1;
            break;
        case plazza::Pizza::PizzaType::Americana:
            _ingredients["gruyere"] -= 1;
            _ingredients["steak"] -= 1;
            break;
        case plazza::Pizza::PizzaType::Fantasia:
            _ingredients["eggplant"] -= 1;
            _ingredients["goat cheese"] -= 1;
            _ingredients["chief love"] -= 1;
            break;
    }
}

void Kitchen::processPizzaQueue() {
    std::unique_lock<std::mutex> lock(_queueMutex);

    if (_pizzaQueue.empty()) {
        return;
    }

    plazza::Pizza pizza = _pizzaQueue.front();
    _pizzaQueue.pop();
    lock.unlock();

    _cooks.enqueue([this, pizza]() { this->cookPizza(pizza); });
}

}  // namespace kitchen
}  // namespace plazza
