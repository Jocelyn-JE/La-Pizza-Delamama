/*
** EPITECH PROJECT, 2025
** La-Pizza-Delamama
** File description:
** Reception
*/

#ifndef SRC_PLAZZA_RECEPTION_RECEPTION_HPP_
#define SRC_PLAZZA_RECEPTION_RECEPTION_HPP_
#include <algorithm>
#include <atomic>
#include <mutex>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

#include "../../NamedPipe.hpp"
#include "../Pizza.hpp"

namespace plazza {
class Reception {
 public:
    Reception() = delete;
    Reception(double cookingMultiplier, unsigned int cookNb,
        unsigned int restockTime);
    ~Reception();
    bool processOrder(const std::string &order);
    void displayStatus() const;

 protected:
 private:
    double _cookingMultiplier;
    unsigned int _cookNb;
    unsigned int _restockTime;
    std::atomic<unsigned int> _nextKitchenId{0};
    mutable std::mutex _kitchensMutex;
    std::vector<pid_t> _kitchenPids;
    std::unordered_map<unsigned int, std::pair<NamedPipe *, NamedPipe *>>
        _kitchenPipes;
    std::atomic<bool> _running{true};
    std::thread _resultCollectorThread;

    bool validatePizza(const std::string &pizza);
    void createKitchen(unsigned int kitchenId);
    void runKitchenProcess(unsigned int kitchenId,
        const std::string &inPipePath, const std::string &outPipePath);
    void dispatchPizza(const Pizza &pizza);
    unsigned int findAvailableKitchen();
    void collectResults();
    Pizza createPizza(const std::string &type, const std::string &size,
        const std::string &count);
};
}  // namespace plazza

#endif  // SRC_PLAZZA_RECEPTION_RECEPTION_HPP_
