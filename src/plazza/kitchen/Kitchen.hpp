/*
** EPITECH PROJECT, 2025
** Plazza
** File description:
** Kitchen
*/

#ifndef SRC_PLAZZA_KITCHEN_KITCHEN_HPP_
#define SRC_PLAZZA_KITCHEN_KITCHEN_HPP_

#include <atomic>
#include <ctime>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

#include "../../SafeQueue.hpp"
#include "../../plazza/Pizza.hpp"

namespace plazza {

class Cook;
class Ingredients {
 public:
    Ingredients() = default;
    ~Ingredients() = default;

    void restock() {
        dough += 5;
        tomato += 5;
        gruyere += 5;
        ham += 5;
        mushrooms += 5;
        steak += 5;
        eggplant += 5;
        goatCheese += 5;
        chiefLove += 5;
    }

    uint16_t dough = 5;
    uint16_t tomato = 5;
    uint16_t gruyere = 5;
    uint16_t ham = 5;
    uint16_t mushrooms = 5;
    uint16_t steak = 5;
    uint16_t eggplant = 5;
    uint16_t goatCheese = 5;
    uint16_t chiefLove = 5;
};

struct KitchenStatus {
    unsigned int busyCooks;
    unsigned int queueSize;
    Ingredients ingredients;
};

class CookState {
 public:
    CookState()
        : isCooking(false),
          hasQueued(false),
          currentPizza(plazza::Pizza::NONE_TYPE, plazza::Pizza::NONE_SIZE),
          queuedPizza(plazza::Pizza::NONE_TYPE, plazza::Pizza::NONE_SIZE) {}

    CookState(const CookState &) = delete;
    CookState &operator=(const CookState &) = delete;

    CookState(CookState &&other) noexcept
        : isCooking(other.isCooking),
          hasQueued(other.hasQueued),
          currentPizza(std::move(other.currentPizza)),
          queuedPizza(std::move(other.queuedPizza)) {
        other.isCooking = false;
        other.hasQueued = false;
    }

    bool isCooking;
    bool hasQueued;
    plazza::Pizza currentPizza;
    plazza::Pizza queuedPizza;

    bool canAcceptPizza() const {
        return !isCooking || !hasQueued;
    }

    int getCurrentLoad() const {
        int load = 0;
        if (isCooking)
            load++;
        if (hasQueued)
            load++;
        return load;
    }
};

class Kitchen {
 public:
    Kitchen(unsigned int cookingMultiplier, unsigned int cookNb,
        unsigned int restockTime, std::string kitchenName);
    ~Kitchen();

    bool assignPizzaToCook(const plazza::Pizza &pizza);
    KitchenStatus getCurrentStatus() const;
    unsigned int getCurrentLoad() const;

    bool isOpen() const {
        return _kitchenOpen;
    }

    unsigned int getCookingMultiplier() const {
        return _cookingMultiplier;
    }

    unsigned int getCookNb() const {
        return _cookNb;
    }

    unsigned int getRestockTime() const {
        return _restockTime;
    }

    std::string getKitchenName() const {
        return _kitchenName;
    }

    bool decrementIngredients(const plazza::Pizza &pizza);

 private:
    void restockWorker();

    unsigned int _cookingMultiplier;
    unsigned int _cookNb;
    unsigned int _restockTime;
    std::string _kitchenName;

    std::vector<std::unique_ptr<Cook>> _cooks;
    std::vector<std::unique_ptr<std::mutex>> _cookMutexes;

    Ingredients _ingredients;
    mutable std::mutex _ingredientsMutex;

    std::atomic<bool> _kitchenOpen{true};
    std::atomic<bool> _running{true};

    std::thread _restockThread;
};

}  // namespace plazza

#endif  // SRC_PLAZZA_KITCHEN_KITCHEN_HPP_
