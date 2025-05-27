/*
** EPITECH PROJECT, 2025
** Kitchen.hpp
** File description:
** _
*/

#ifndef SRC_PLAZZA_KITCHEN_KITCHEN_HPP_
#define SRC_PLAZZA_KITCHEN_KITCHEN_HPP_

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <mutex>
#include <queue>
#include <thread>
#include <unordered_map>

#include "../../NamedPipe.hpp"
#include "../../ThreadPool.hpp"
#include "../Pizza.hpp"

namespace plazza {
namespace kitchen {

struct KitchenStatus {
    unsigned int busyCooks;
    unsigned int queueSize;
    std::unordered_map<std::string, unsigned int> ingredients;
};

class Kitchen {
 public:
    Kitchen(unsigned int id, unsigned int cookNb, unsigned int restockTime,
        double cookingMultiplier);
    ~Kitchen();

    void run();
    bool addPizza(const plazza::Pizza &pizza);

    unsigned int getId() const {
        return _id;
    }

    size_t getQueueSize() const;
    size_t getBusyCooks() const;
    const std::unordered_map<std::string, unsigned int> &getIngredients()
        const;

 private:
    unsigned int _id;
    double _cookingMultiplier;
    unsigned int _restockTime;
    ThreadPool _cooks;
    std::unordered_map<std::string, unsigned int> _ingredients;
    std::queue<plazza::Pizza> _pizzaQueue;
    std::atomic<bool> _running;
    std::atomic<bool> _hasWork;
    std::chrono::time_point<std::chrono::steady_clock> _lastWorkTime;
    mutable std::mutex _queueMutex;
    std::mutex _ingredientsMutex;
    std::condition_variable _queueCondition;
    std::thread _restockThread;
    std::thread _workWatcherThread;
    plazza::ipc::NamedPipe *_inPipe;
    plazza::ipc::NamedPipe *_outPipe;

    void restockIngredients();
    void watchForWork();
    bool cookPizza(const plazza::Pizza &pizza);
    bool hasEnoughIngredients(const plazza::Pizza &pizza);
    void useIngredients(const plazza::Pizza &pizza);
    void processPizzaQueue();
    void handleCommand();
};

}  // namespace kitchen
}  // namespace plazza

#endif  // SRC_PLAZZA_KITCHEN_KITCHEN_HPP_
