/*
** EPITECH PROJECT, 2025
** La-Pizza-Delamama
** File description:
** Reception
*/

#include "Reception.hpp"
#include <signal.h>
#include <sys/wait.h>
#include <unistd.h>

#include <iostream>
#include <limits>
#include <sstream>
#include <string>
#include <unordered_set>
#include <vector>

#include "../../Utils.hpp"

plazza::Reception::Reception(
    double cookingMultiplier, unsigned int cookNb, unsigned int restockTime)
    : _cookingMultiplier(cookingMultiplier),
      _cookNb(cookNb),
      _restockTime(restockTime),
      _nextKitchenId(0),
      _running(true) {
    _resultCollectorThread = std::thread(&Reception::collectResults, this);
}

plazza::Reception::~Reception() {
    _running = false;

    if (_resultCollectorThread.joinable()) {
        _resultCollectorThread.join();
    }

    for (auto &pair : _kitchenPipes) {
        delete pair.second.first;
        delete pair.second.second;
    }

    for (pid_t pid : _kitchenPids) {
        kill(pid, SIGTERM);
    }

    for (pid_t pid : _kitchenPids) {
        waitpid(pid, nullptr, 0);
    }
}

bool plazza::Reception::processOrder(const std::string &order) {
    std::vector<std::string> pizzaOrders = utils::split(order, "; ");

    for (auto &pizza : pizzaOrders) {
        std::cout << "Processing pizza order: " << pizza << std::endl;
        if (pizza.empty()) {
            std::cout << "Empty pizza order" << std::endl;
            return false;
        }
        if (!validatePizza(pizza)) {
            return false;
        }
        std::cout << "valid pizza" << std::endl;

        std::vector<std::string> tokenizedPizza = utils::split(pizza, " ");
        Pizza pizzaObj = createPizza(
            tokenizedPizza[0], tokenizedPizza[1], tokenizedPizza[2]);

        std::cout << "Created pizza: " << pizzaObj.toString() << std::endl;
        std::string countStr = tokenizedPizza[2].substr(1);
        unsigned int count = std::stoi(countStr);

        std::cout << "dispatching pizza" << std::endl;
        for (unsigned int i = 0; i < count; ++i) {
            dispatchPizza(pizzaObj);
        }
        std::cout << "dispatched pizza" << std::endl;
    }

    return true;
}

void plazza::Reception::displayStatus() const {
    // std::cout << "Plazza Reception Status:" << std::endl;
    // std::lock_guard<std::mutex> lock(_kitchensMutex);

    std::cout << "Kitchen Status:" << std::endl;
    std::cout << "---------------" << std::endl;

    if (_kitchenPids.empty()) {
        std::cout << "No kitchens running." << std::endl;
        return;
    }

    for (unsigned int i = 0; i < _kitchenPids.size(); ++i) {
        std::cout << "Kitchen " << i << ":" << std::endl;

        auto it = _kitchenPipes.find(i);
        if (it != _kitchenPipes.end()) {
            ipc::NamedPipe *inPipe = it->second.first;
            ipc::NamedPipe *outPipe = it->second.second;

            int statusRequest = 1;
            inPipe->write(statusRequest);

            kitchen::KitchenStatus status;
            if (outPipe->read(status)) {
                std::cout << "  Busy Cooks: " << status.busyCooks << "/"
                          << _cookNb << std::endl;
                std::cout << "  Queue Size: " << status.queueSize << std::endl;
                std::cout << "  Ingredients:" << std::endl;

                for (const auto &ingredient : status.ingredients) {
                    std::cout << "    " << ingredient.first << ": "
                              << ingredient.second << std::endl;
                }
            } else {
                std::cout << "  Unable to get status." << std::endl;
            }
        } else {
            std::cout << "  Unable to communicate with kitchen." << std::endl;
        }
    }
}

bool plazza::Reception::validatePizza(const std::string &pizza) {
    std::vector<std::string> tokenizedPizza = utils::split(pizza, " ");

    if (tokenizedPizza.size() != 3) {
        std::cout << (tokenizedPizza.size() < 3 ? "Too few" : "Too many")
                  << " values: " << pizza << std::endl;
        std::cout << "Expected format: <type> <size> <count>" << std::endl;
        return false;
    }
    if (!utils::isValidPizzaType(tokenizedPizza[0])) {
        std::cout << "Invalid pizza type: " << tokenizedPizza[0] << std::endl;
        std::cout << "Valid types are: ";
        for (const auto &type : plazza::validPizzaTypes)
            std::cout << type << " ";
        std::cout << std::endl;
        return false;
    }
    if (!utils::isValidPizzaSize(tokenizedPizza[1])) {
        std::cout << "Invalid pizza size: " << tokenizedPizza[1] << std::endl;
        std::cout << "Valid sizes are: ";
        for (const auto &size : plazza::validPizzaSizes)
            std::cout << size << " ";
        std::cout << std::endl;
        return false;
    }
    if (!utils::isValidPizzaCount(tokenizedPizza[2])) {
        std::cout << "Invalid pizza count: " << tokenizedPizza[2] << std::endl;
        std::cout
            << "Count should be in the format 'xN' where N is a positive "
               "integer"
            << std::endl;
        return false;
    }
    return true;
}

void plazza::Reception::createKitchen() {
    unsigned int kitchenId = _nextKitchenId++;

    std::string inPipeName =
        "/tmp/kitchen_" + std::to_string(kitchenId) + "_in";
    std::string outPipeName =
        "/tmp/kitchen_" + std::to_string(kitchenId) + "_out";

    std::cout << "Creating named pipes for kitchen " << kitchenId << std::endl;

    ipc::NamedPipe *inPipe =
        new ipc::NamedPipe(inPipeName, ipc::NamedPipe::WRITE);
    ipc::NamedPipe *outPipe =
        new ipc::NamedPipe(outPipeName, ipc::NamedPipe::READ);

    std::cout << "created named pipes for kitchen " << kitchenId << std::endl;

    pid_t pid = fork();

    std::cout << "Forking new kitchen process with ID " << kitchenId
              << std::endl;
    if (pid == -1) {
        std::cerr << "Failed to fork a new kitchen process." << std::endl;
        delete inPipe;
        delete outPipe;
        return;
    }

    if (pid == 0) {
        inPipe->openPipe();
        outPipe->openPipe();
        std::cout << "kitchen child process started with ID " << kitchenId
                  << std::endl;
        kitchen::Kitchen kitchen(
            kitchenId, _cookNb, _restockTime, _cookingMultiplier);
        kitchen.run();
        exit(0);
    } else {
        inPipe->openPipe();
        outPipe->openPipe();
        std::cout << "kitchen parent process registered kitchen with ID "
                  << kitchenId << std::endl;
        std::lock_guard<std::mutex> lock(_kitchensMutex);
        _kitchenPids.push_back(pid);
        _kitchenPipes[kitchenId] = std::make_pair(inPipe, outPipe);

        std::cout << "Created new kitchen with ID " << kitchenId << std::endl;
    }
}

void plazza::Reception::dispatchPizza(const Pizza &pizza) {
    unsigned int kitchenId = findAvailableKitchen();

    std::cout << "Dispatching pizza to kitchen " << kitchenId << std::endl;
    std::lock_guard<std::mutex> lock(_kitchensMutex);
    auto it = _kitchenPipes.find(kitchenId);
    if (it != _kitchenPipes.end()) {
        ipc::NamedPipe *pipe = it->second.first;

        int command = 3;
        pipe->write(command);

        pipe->write(pizza);

        std::cout << "Dispatched " << pizza.toString() << " to kitchen "
                  << kitchenId << std::endl;
    }
}

unsigned int plazza::Reception::findAvailableKitchen() {
    // std::unique_lock<std::mutex> lock(_kitchensMutex);

    if (_kitchenPids.empty()) {
        std::cout << "No kitchens available, creating a new one..."
                  << std::endl;
        // lock.unlock();
        createKitchen();
        return 0;
    }

    std::cout << "Finding available kitchen..." << std::endl;
    unsigned int minLoad = std::numeric_limits<unsigned int>::max();
    unsigned int minLoadKitchenId = 0;
    bool allFull = true;

    for (const auto &pair : _kitchenPipes) {
        unsigned int kitchenId = pair.first;
        ipc::NamedPipe *inPipe = pair.second.first;
        ipc::NamedPipe *outPipe = pair.second.second;

        int loadRequest = 2;
        inPipe->write(loadRequest);

        unsigned int load;
        if (outPipe->read(load)) {
            if (load < 2 * _cookNb) {
                allFull = false;
            }

            if (load < minLoad) {
                minLoad = load;
                minLoadKitchenId = kitchenId;
            }
        }
    }

    if (allFull) {
        // lock.unlock();
        createKitchen();
        return _nextKitchenId - 1;
    }

    return minLoadKitchenId;
}

void plazza::Reception::collectResults() {
    while (_running) {
        std::lock_guard<std::mutex> lock(_kitchensMutex);

        for (auto it = _kitchenPipes.begin(); it != _kitchenPipes.end();) {
            ipc::NamedPipe *outPipe = it->second.second;

            Pizza pizza;
            if (outPipe->read(pizza)) {
                std::cout << "Pizza ready: " << pizza.toString() << std::endl;
            }

            int status;
            pid_t result = waitpid(_kitchenPids[it->first], &status, WNOHANG);

            if (result > 0) {
                std::cout << "Kitchen " << it->first << " has closed."
                          << std::endl;
                delete it->second.first;
                delete it->second.second;
                it = _kitchenPipes.erase(it);
            } else {
                ++it;
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

plazza::Pizza plazza::Reception::createPizza(const std::string &type,
    const std::string &size, const std::string &count) {
    (void)count;
    Pizza::PizzaType pizzaType;
    Pizza::PizzaSize pizzaSize;

    if (type == "regina") {
        pizzaType = Pizza::PizzaType::Regina;
    } else if (type == "margarita") {
        pizzaType = Pizza::PizzaType::Margarita;
    } else if (type == "americana") {
        pizzaType = Pizza::PizzaType::Americana;
    } else if (type == "fantasia") {
        pizzaType = Pizza::PizzaType::Fantasia;
    }

    std::string upperSize = utils::toUpper(size);
    if (upperSize == "S") {
        pizzaSize = Pizza::PizzaSize::S;
    } else if (upperSize == "M") {
        pizzaSize = Pizza::PizzaSize::M;
    } else if (upperSize == "L") {
        pizzaSize = Pizza::PizzaSize::L;
    } else if (upperSize == "XL") {
        pizzaSize = Pizza::PizzaSize::XL;
    } else if (upperSize == "XXL") {
        pizzaSize = Pizza::PizzaSize::XXL;
    }

    return Pizza(pizzaType, pizzaSize);
}
