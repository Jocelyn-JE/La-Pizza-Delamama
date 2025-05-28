/*
** EPITECH PROJECT, 2025
** La-Pizza-Delamama
** File description:
** Reception
*/

#include "./Reception.hpp"
#include <signal.h>
#include <sys/wait.h>
#include <unistd.h>

#include <chrono>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>
#include <thread>
#include <unordered_set>
#include <vector>

#include "../../Utils.hpp"
#include "../kitchen/Kitchen.hpp"

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

    std::lock_guard<std::mutex> lock(_kitchensMutex);
    for (auto &pair : _kitchenPipes) {
        try {
            KitchenCommand cmd;
            cmd.type = KitchenCommand::SHUTDOWN;
            pair.second.first->write(cmd);
        } catch (...) {}
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

        std::vector<std::string> tokenizedPizza = utils::split(pizza, " ");
        Pizza pizzaObj = createPizza(
            tokenizedPizza[0], tokenizedPizza[1], tokenizedPizza[2]);

        std::cout << "Created pizza: " << pizzaObj.toString() << std::endl;
        std::string countStr = tokenizedPizza[2].substr(1);
        unsigned int count = std::stoi(countStr);

        std::cout << "Dispatching " << count << " pizzas" << std::endl;
        for (unsigned int i = 0; i < count; ++i) {
            dispatchPizza(pizzaObj);
        }
        std::cout << "Dispatched pizzas" << std::endl;
    }

    return true;
}

void plazza::Reception::displayStatus() const {
    std::cout << "Kitchen Status:" << std::endl;
    std::cout << "---------------" << std::endl;

    std::lock_guard<std::mutex> lock(_kitchensMutex);

    if (_kitchenPids.empty()) {
        std::cout << "No kitchens running." << std::endl;
        return;
    }

    for (auto &pair : _kitchenPipes) {
        unsigned int kitchenId = pair.first;
        NamedPipe *receptionToKitchen = pair.second.first;
        NamedPipe *kitchenToReception = pair.second.second;

        std::cout << "Kitchen " << kitchenId << ":" << std::endl;

        try {
            KitchenCommand cmd;
            cmd.type = KitchenCommand::STATUS_REQUEST;
            if (receptionToKitchen->write(cmd)) {
                KitchenStatus status;
                if (kitchenToReception->read(status)) {
                    std::cout << "  Busy Cooks: " << status.busyCooks << "/"
                              << _cookNb << std::endl;
                    std::cout << "  Queue Size: " << status.queueSize
                              << std::endl;
                    std::cout << "  Ingredients:" << std::endl;
                    std::cout << "    Dough: " << status.ingredients.dough
                              << std::endl;
                    std::cout << "    Tomato: " << status.ingredients.tomato
                              << std::endl;
                    std::cout << "    Gruyere: " << status.ingredients.gruyere
                              << std::endl;
                    std::cout << "    Ham: " << status.ingredients.ham
                              << std::endl;
                    std::cout
                        << "    Mushrooms: " << status.ingredients.mushrooms
                        << std::endl;
                    std::cout << "    Steak: " << status.ingredients.steak
                              << std::endl;
                    std::cout
                        << "    Eggplant: " << status.ingredients.eggplant
                        << std::endl;
                    std::cout
                        << "    Goat Cheese: " << status.ingredients.goatCheese
                        << std::endl;
                    std::cout
                        << "    Chief Love: " << status.ingredients.chiefLove
                        << std::endl;
                } else {
                    std::cout << "  Unable to get status." << std::endl;
                }
            } else {
                std::cout << "  Unable to send status request." << std::endl;
            }
        } catch (const std::exception &e) {
            std::cout << "  Error communicating with kitchen: " << e.what()
                      << std::endl;
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

    std::string receptionToKitchenPipe =
        "/tmp/reception_to_kitchen_" + std::to_string(kitchenId);
    std::string kitchenToReceptionPipe =
        "/tmp/kitchen_to_reception_" + std::to_string(kitchenId);

    std::cout << "Creating kitchen " << kitchenId << std::endl;

    try {
        NamedPipe *receptionToKitchen = new NamedPipe(receptionToKitchenPipe);
        NamedPipe *kitchenToReception = new NamedPipe(kitchenToReceptionPipe);

        pid_t pid = fork();

        if (pid == -1) {
            std::cerr << "Failed to fork a new kitchen process." << std::endl;
            delete receptionToKitchen;
            delete kitchenToReception;
            return;
        }

        if (pid == 0) {
            std::cout << "Kitchen child process " << kitchenId << " started"
                      << std::endl;

            runKitchenProcess(
                kitchenId, receptionToKitchenPipe, kitchenToReceptionPipe);

            delete receptionToKitchen;
            delete kitchenToReception;
            exit(0);
        } else {
            std::cout << "Created kitchen with ID " << kitchenId
                      << " (PID: " << pid << ")" << std::endl;

            std::lock_guard<std::mutex> lock(_kitchensMutex);
            _kitchenPids.push_back(pid);
            _kitchenPipes[kitchenId] =
                std::make_pair(receptionToKitchen, kitchenToReception);
        }
    } catch (const std::exception &e) {
        std::cerr << "Failed to create kitchen " << kitchenId << ": "
                  << e.what() << std::endl;
    }
}

void plazza::Reception::runKitchenProcess(unsigned int kitchenId,
    const std::string &inPipePath, const std::string &outPipePath) {
    kitchen::Kitchen kitchen(_cookingMultiplier, _cookNb, _restockTime,
        "Kitchen_" + std::to_string(kitchenId));

    NamedPipe inPipe(inPipePath);    // Reception to Kitchen
    NamedPipe outPipe(outPipePath);  // Kitchen to Reception

    bool running = true;
    auto lastActivity = std::chrono::steady_clock::now();

    // Start kitchen cooking threads
    std::thread kitchenThread([&kitchen, &running, &lastActivity]() {
        kitchen.startCooks();
        while (running) {
            // Check if kitchen should close due to inactivity
            auto now = std::chrono::steady_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::seconds>(
                now - lastActivity);
            if (duration.count() >= 5) {
                std::cout << "Kitchen " << kitchen.getKitchenName()
                          << " closing due to inactivity" << std::endl;
                running = false;
                break;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        kitchen.stopCooks();
    });

    while (running) {
        try {
            KitchenCommand cmd;
            if (inPipe.read(cmd)) {
                lastActivity = std::chrono::steady_clock::now();

                switch (cmd.type) {
                    case KitchenCommand::STATUS_REQUEST: {
                        KitchenStatus status = kitchen.getStatus();
                        outPipe.write(status);
                        break;
                    }
                    case KitchenCommand::LOAD_REQUEST: {
                        LoadResponse load;
                        load.currentLoad = kitchen.getCurrentLoad();
                        outPipe.write(load);
                        break;
                    }
                    case KitchenCommand::PIZZA_ORDER: {
                        Pizza pizza;
                        if (inPipe.readPizza(pizza)) {
                            if (kitchen.addPizza(pizza)) {
                                std::cout
                                    << "Kitchen " << kitchenId
                                    << " accepted pizza: " << pizza.toString()
                                    << std::endl;
                            } else {
                                std::cout
                                    << "Kitchen " << kitchenId
                                    << " rejected pizza: " << pizza.toString()
                                    << std::endl;
                            }
                        }
                        break;
                    }
                    case KitchenCommand::SHUTDOWN:
                        running = false;
                        break;
                }
            }

            // Check for completed pizzas
            Pizza completedPizza;
            while (kitchen.getCompletedPizza(completedPizza)) {
                std::cout << "Pizza completed in kitchen " << kitchenId << ": "
                          << completedPizza.toString() << std::endl;
                outPipe.writePizza(completedPizza);
            }

        } catch (const std::exception &e) {
            std::cerr << "Kitchen " << kitchenId
                      << " communication error: " << e.what() << std::endl;
            running = false;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }

    if (kitchenThread.joinable()) {
        kitchenThread.join();
    }

    std::cout << "Kitchen " << kitchenId << " process ended" << std::endl;
}

void plazza::Reception::dispatchPizza(const Pizza &pizza) {
    unsigned int kitchenId = findAvailableKitchen();

    std::cout << "Dispatching pizza to kitchen " << kitchenId << std::endl;

    std::lock_guard<std::mutex> lock(_kitchensMutex);
    auto it = _kitchenPipes.find(kitchenId);
    if (it != _kitchenPipes.end()) {
        NamedPipe *receptionToKitchen = it->second.first;

        try {
            KitchenCommand cmd;
            cmd.type = KitchenCommand::PIZZA_ORDER;
            if (receptionToKitchen->write(cmd) &&
                receptionToKitchen->writePizza(pizza)) {
                std::cout << "Dispatched " << pizza.toString()
                          << " to kitchen " << kitchenId << std::endl;
            } else {
                std::cout << "Failed to dispatch pizza to kitchen "
                          << kitchenId << std::endl;
            }
        } catch (const std::exception &e) {
            std::cerr << "Error dispatching pizza to kitchen " << kitchenId
                      << ": " << e.what() << std::endl;
        }
    }
}

unsigned int plazza::Reception::findAvailableKitchen() {
    std::unique_lock<std::mutex> lock(_kitchensMutex);

    if (_kitchenPipes.empty()) {
        std::cout << "No kitchens available, creating a new one..."
                  << std::endl;
        lock.unlock();
        createKitchen();
        return _nextKitchenId - 1;
    }

    std::cout << "Finding available kitchen..." << std::endl;
    unsigned int minLoad = std::numeric_limits<unsigned int>::max();
    unsigned int minLoadKitchenId = 0;
    bool allFull = true;

    for (const auto &pair : _kitchenPipes) {
        unsigned int kitchenId = pair.first;
        NamedPipe *receptionToKitchen = pair.second.first;
        NamedPipe *kitchenToReception = pair.second.second;

        try {
            KitchenCommand cmd;
            cmd.type = KitchenCommand::LOAD_REQUEST;
            if (receptionToKitchen->write(cmd)) {
                LoadResponse loadResponse;
                if (kitchenToReception->read(loadResponse)) {
                    unsigned int load = loadResponse.currentLoad;

                    if (load < 2 * _cookNb) {
                        allFull = false;
                    }

                    if (load < minLoad) {
                        minLoad = load;
                        minLoadKitchenId = kitchenId;
                    }
                }
            }
        } catch (const std::exception &e) {
            std::cerr << "Error checking kitchen " << kitchenId
                      << " load: " << e.what() << std::endl;
        }
    }

    if (allFull) {
        lock.unlock();
        createKitchen();
        return _nextKitchenId - 1;
    }

    return minLoadKitchenId;
}

void plazza::Reception::collectResults() {
    while (_running) {
        std::lock_guard<std::mutex> lock(_kitchensMutex);

        for (auto it = _kitchenPipes.begin(); it != _kitchenPipes.end();) {
            NamedPipe *kitchenToReception = it->second.second;
            unsigned int kitchenId = it->first;

            try {
                Pizza pizza;
                if (kitchenToReception->readPizza(pizza)) {
                    std::cout << "Pizza ready from kitchen " << kitchenId
                              << ": " << pizza.toString() << std::endl;
                }
            } catch (const std::exception &e) {}

            auto pidIt = std::find(_kitchenPids.begin(), _kitchenPids.end(),
                _kitchenPids[std::distance(_kitchenPipes.begin(), it)]);
            if (pidIt != _kitchenPids.end()) {
                int status;
                pid_t result = waitpid(*pidIt, &status, WNOHANG);

                if (result > 0) {
                    std::cout << "Kitchen " << kitchenId << " has closed."
                              << std::endl;
                    delete it->second.first;
                    delete it->second.second;
                    _kitchenPids.erase(pidIt);
                    it = _kitchenPipes.erase(it);
                    continue;
                }
            }
            ++it;
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
    } else {
        pizzaType = Pizza::PizzaType::NONE_TYPE;
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
    } else {
        pizzaSize = Pizza::PizzaSize::NONE_SIZE;
    }

    return Pizza(pizzaType, pizzaSize);
}
