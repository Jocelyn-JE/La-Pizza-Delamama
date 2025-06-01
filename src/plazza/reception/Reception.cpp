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
      _restockTime(restockTime) {
    signal(SIGPIPE, SIG_IGN);

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
            pair.second.first->writeData(cmd);
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

    system("rm -f /tmp/reception_to_kitchen_* /tmp/kitchen_to_reception_*");
}

bool plazza::Reception::processOrder(const std::string &order) {
    std::vector<std::string> pizzaOrders = utils::split(order, "; ");

    for (auto &pizza : pizzaOrders) {
        std::cerr << "Processing pizza order: " << pizza << std::endl;
        if (pizza.empty()) {
            std::cerr << "Empty pizza order" << std::endl;
            return false;
        }
        if (!validatePizza(pizza)) {
            return false;
        }

        std::vector<std::string> tokenizedPizza = utils::split(pizza, " ");
        Pizza pizzaObj = createPizza(
            tokenizedPizza[0], tokenizedPizza[1], tokenizedPizza[2]);

        std::cerr << "Created pizza: " << pizzaObj.toString() << std::endl;
        std::string countStr = tokenizedPizza[2].substr(1);
        unsigned int count = std::stoi(countStr);

        std::cerr << "Dispatching " << count << " pizzas" << std::endl;
        for (unsigned int i = 0; i < count; ++i) {
            dispatchPizza(pizzaObj);
        }
        std::cerr << "Dispatched pizzas" << std::endl;
    }

    return true;
}

void plazza::Reception::displayStatus() const {
    std::cout << "Kitchen Status:" << std::endl;
    std::cout << "---------------" << std::endl;

    std::unordered_map<unsigned int, std::pair<NamedPipe *, NamedPipe *>>
        kitchensCopy;

    {
        std::lock_guard<std::mutex> lock(_kitchensMutex);
        kitchensCopy = _kitchenPipes;
    }

    if (kitchensCopy.empty()) {
        std::cout << "No kitchens running." << std::endl;
        return;
    }

    std::cout << "Found " << kitchensCopy.size() << " kitchen(s)" << std::endl;

    for (auto &pair : kitchensCopy) {
        unsigned int kitchenId = pair.first;
        NamedPipe *receptionToKitchen = pair.second.first;
        NamedPipe *kitchenToReception = pair.second.second;

        std::cerr << "Kitchen " << kitchenId << ":" << std::endl;

        try {
            KitchenCommand cmd;
            cmd.type = KitchenCommand::STATUS_REQUEST;

            if (receptionToKitchen->writeData(cmd)) {
                KitchenStatus status;

                if (kitchenToReception->readData(status)) {
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
                    std::cout << "  Unable to get status response."
                              << std::endl;
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

void plazza::Reception::createKitchen(unsigned int kitchenId) {
    std::string receptionToKitchenPipe =
        "/tmp/reception_to_kitchen_" + std::to_string(kitchenId);
    std::string kitchenToReceptionPipe =
        "/tmp/kitchen_to_reception_" + std::to_string(kitchenId);

    std::cerr << "Creating kitchen " << kitchenId
              << " (capacity: " << (2 * _cookNb) << " pizzas)" << std::endl;

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
            std::cerr << "Kitchen child process " << kitchenId << " started"
                      << std::endl;

            runKitchenProcess(
                kitchenId, receptionToKitchenPipe, kitchenToReceptionPipe);

            delete receptionToKitchen;
            delete kitchenToReception;
            exit(0);
        } else {
            std::cerr << "Created kitchen with ID " << kitchenId
                      << " (PID: " << pid << ")" << std::endl;

            _kitchenPids.push_back(pid);
            _kitchenPipes[kitchenId] =
                std::make_pair(receptionToKitchen, kitchenToReception);

            std::this_thread::sleep_for(std::chrono::milliseconds(300));
        }
    } catch (const std::exception &e) {
        std::cerr << "Failed to create kitchen " << kitchenId << ": "
                  << e.what() << std::endl;
    }
}

void plazza::Reception::runKitchenProcess(unsigned int kitchenId,
    const std::string &inPipePath, const std::string &outPipePath) {
    signal(SIGPIPE, SIG_IGN);

    std::cerr << "DEBUG: Kitchen " << kitchenId
              << " starting runKitchenProcess" << std::endl;

    plazza::Kitchen kitchen(static_cast<unsigned int>(_cookingMultiplier),
        _cookNb, _restockTime, "Kitchen_" + std::to_string(kitchenId));

    std::cerr << "DEBUG: Kitchen " << kitchenId << " created Kitchen object"
              << std::endl;

    NamedPipe inPipe(inPipePath);
    NamedPipe outPipe(outPipePath);

    std::cerr << "DEBUG: Kitchen " << kitchenId
              << " created pipes, entering main loop" << std::endl;

    bool running = true;
    auto lastActivity = std::chrono::steady_clock::now();

    std::cerr << "Kitchen " << kitchenId
              << " is ready and waiting for commands" << std::endl;

    while (running) {
        try {
            KitchenCommand cmd;
            if (inPipe.readData(cmd)) {
                lastActivity = std::chrono::steady_clock::now();

                std::cerr << "Kitchen " << kitchenId
                          << " received command type: " << cmd.type
                          << std::endl;

                switch (cmd.type) {
                    case KitchenCommand::STATUS_REQUEST: {
                        std::cerr << "Kitchen " << kitchenId
                                  << " processing STATUS_REQUEST" << std::endl;
                        KitchenStatus status = kitchen.getCurrentStatus();
                        if (outPipe.writeData(status)) {
                            std::cerr << "Kitchen " << kitchenId
                                      << " sent status response" << std::endl;
                        } else {
                            std::cerr << "Kitchen " << kitchenId
                                      << " failed to send status response"
                                      << std::endl;
                        }
                        break;
                    }
                    case KitchenCommand::LOAD_REQUEST: {
                        std::cerr << "Kitchen " << kitchenId
                                  << " processing LOAD_REQUEST" << std::endl;
                        LoadResponse load;
                        load.currentLoad = kitchen.getCurrentLoad();
                        if (outPipe.writeData(load)) {
                            std::cerr
                                << "Kitchen " << kitchenId
                                << " sent load response: " << load.currentLoad
                                << std::endl;
                        } else {
                            std::cerr << "Kitchen " << kitchenId
                                      << " failed to send load response"
                                      << std::endl;
                        }
                        break;
                    }
                    case KitchenCommand::PIZZA_ORDER: {
                        std::cerr << "Kitchen " << kitchenId
                                  << " processing PIZZA_ORDER" << std::endl;

                        std::cerr << "Pizza data: type=" << cmd.pizzaData.type
                                  << ", size=" << cmd.pizzaData.size
                                  << std::endl;

                        plazza::Pizza pizza(
                            static_cast<plazza::Pizza::PizzaType>(
                                cmd.pizzaData.type),
                            static_cast<plazza::Pizza::PizzaSize>(
                                cmd.pizzaData.size));

                        if (kitchen.assignPizzaToCook(pizza)) {
                            std::cerr
                                << "Kitchen " << kitchenId
                                << " accepted pizza: " << pizza.toString()
                                << " (current load: "
                                << kitchen.getCurrentLoad() << "/"
                                << (2 * _cookNb) << ")" << std::endl;
                        } else {
                            std::cerr
                                << "Kitchen " << kitchenId
                                << " rejected pizza: " << pizza.toString()
                                << " (all cooks full)" << std::endl;
                        }
                        break;
                    }
                    case KitchenCommand::SHUTDOWN:
                        std::cerr << "Kitchen " << kitchenId
                                  << " received SHUTDOWN command" << std::endl;
                        running = false;
                        break;
                    default:
                        std::cerr << "Kitchen " << kitchenId
                                  << " received unknown command: " << cmd.type
                                  << std::endl;
                        break;
                }
            } else {
                // No command received, continue loop
            }

        } catch (const std::exception &e) {
            std::cerr << "Kitchen " << kitchenId
                      << " communication error: " << e.what() << std::endl;
        }

        auto now = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(
            now - lastActivity);

        bool hasWork = (kitchen.getCurrentLoad() > 0);

        if (duration.count() >= 5 && !hasWork) {
            std::cout << "Kitchen " << kitchenId
                      << " closing due to inactivity (no work for 5 seconds)"
                      << std::endl;
            running = false;
        } else if (hasWork) {
            lastActivity = now;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    std::cerr << "Kitchen " << kitchenId << " process ended" << std::endl;
}

void plazza::Reception::dispatchPizza(const Pizza &pizza) {
    unsigned int kitchenId = findAvailableKitchen();

    std::cerr << "Dispatching pizza to kitchen " << kitchenId << std::endl;

    NamedPipe *receptionToKitchen = nullptr;

    {
        std::lock_guard<std::mutex> lock(_kitchensMutex);
        auto it = _kitchenPipes.find(kitchenId);
        if (it != _kitchenPipes.end()) {
            receptionToKitchen = it->second.first;
        }
    }

    if (receptionToKitchen) {
        try {
            KitchenCommand cmd;
            cmd.type = KitchenCommand::PIZZA_ORDER;
            cmd.pizzaData.type = static_cast<int>(pizza.getType());
            cmd.pizzaData.size = static_cast<int>(pizza.getSize());

            std::cerr << "Sending pizza order: type=" << cmd.pizzaData.type
                      << ", size=" << cmd.pizzaData.size << std::endl;

            bool dispatchSucceeded = false;
            for (int retry = 0; retry < 3; retry++) {
                if (receptionToKitchen->writeData(cmd)) {
                    dispatchSucceeded = true;
                    break;
                } else {
                    std::cerr << "Failed to dispatch pizza to kitchen "
                              << kitchenId << ", attempt " << (retry + 1)
                              << "/3" << std::endl;

                    if (retry < 2) {
                        std::this_thread::sleep_for(
                            std::chrono::milliseconds(50));
                    }
                }
            }

            if (dispatchSucceeded) {
                std::cerr << "Successfully dispatched " << pizza.toString()
                          << " to kitchen " << kitchenId << std::endl;
            } else {
                std::cerr << "Failed to dispatch pizza after 3 attempts, "
                             "trying different kitchen..."
                          << std::endl;

                {
                    std::lock_guard<std::mutex> lock(_kitchensMutex);
                    auto it = _kitchenPipes.find(kitchenId);
                    if (it != _kitchenPipes.end()) {
                        std::cerr << "Removing problematic kitchen "
                                  << kitchenId << std::endl;
                        delete it->second.first;
                        delete it->second.second;
                        _kitchenPipes.erase(it);
                    }
                }

                dispatchPizza(pizza);
            }
        } catch (const std::exception &e) {
            std::cerr << "Error dispatching pizza to kitchen " << kitchenId
                      << ": " << e.what() << std::endl;

            std::lock_guard<std::mutex> lock(_kitchensMutex);
            unsigned int newId = _nextKitchenId++;
            createKitchen(newId);

            static thread_local int recursion_depth = 0;
            if (recursion_depth < 2) {
                recursion_depth++;
                dispatchPizza(pizza);
                recursion_depth--;
            }
        }
    } else {
        std::cerr << "Kitchen " << kitchenId
                  << " not found, creating new kitchen..." << std::endl;
        std::lock_guard<std::mutex> lock(_kitchensMutex);
        createKitchen(kitchenId);
        dispatchPizza(pizza);
    }
}

unsigned int plazza::Reception::findAvailableKitchen() {
    bool needNewKitchen = false;
    unsigned int newKitchenId = 0;

    {
        std::lock_guard<std::mutex> lock(_kitchensMutex);

        if (_kitchenPipes.empty()) {
            std::cerr << "No kitchens available, creating a new one..."
                      << std::endl;
            newKitchenId = _nextKitchenId++;
            needNewKitchen = true;
        }
    }

    if (needNewKitchen) {
        std::lock_guard<std::mutex> lock(_kitchensMutex);
        createKitchen(newKitchenId);
        return newKitchenId;
    }

    std::cerr << "Finding available kitchen..." << std::endl;
    unsigned int minLoad = std::numeric_limits<unsigned int>::max();
    unsigned int bestKitchenId = 0;
    bool foundAvailable = false;

    std::unordered_map<unsigned int, std::pair<NamedPipe *, NamedPipe *>>
        kitchensCopy;
    {
        std::lock_guard<std::mutex> lock(_kitchensMutex);
        kitchensCopy = _kitchenPipes;
    }

    std::vector<unsigned int> failedKitchens;

    for (const auto &pair : kitchensCopy) {
        unsigned int kitchenId = pair.first;
        NamedPipe *receptionToKitchen = pair.second.first;
        NamedPipe *kitchenToReception = pair.second.second;

        try {
            KitchenCommand cmd;
            cmd.type = KitchenCommand::LOAD_REQUEST;

            bool loadRequestSucceeded = false;
            LoadResponse loadResponse;

            for (int retry = 0; retry < 2; retry++) {
                if (receptionToKitchen->writeData(cmd)) {
                    if (kitchenToReception->readData(loadResponse)) {
                        loadRequestSucceeded = true;
                        break;
                    }
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }

            if (loadRequestSucceeded) {
                unsigned int load = loadResponse.currentLoad;
                unsigned int maxCapacity = 2 * _cookNb;

                std::cerr << "Kitchen " << kitchenId
                          << " current load: " << load << "/" << maxCapacity
                          << std::endl;

                if (load < maxCapacity - 1) {
                    foundAvailable = true;
                    if (load < minLoad) {
                        minLoad = load;
                        bestKitchenId = kitchenId;
                    }
                } else {
                    std::cerr << "Kitchen " << kitchenId
                              << " is full or nearly full" << std::endl;
                }
            } else {
                std::cerr << "Kitchen " << kitchenId
                          << " didn't respond to load request" << std::endl;
                failedKitchens.push_back(kitchenId);
            }
        } catch (const std::exception &e) {
            std::cerr << "Error checking kitchen " << kitchenId
                      << " load: " << e.what() << std::endl;
            failedKitchens.push_back(kitchenId);
        }
    }

    if (!failedKitchens.empty()) {
        std::lock_guard<std::mutex> lock(_kitchensMutex);
        for (unsigned int kitchenId : failedKitchens) {
            auto it = _kitchenPipes.find(kitchenId);
            if (it != _kitchenPipes.end()) {
                std::cerr << "Removing unresponsive kitchen " << kitchenId
                          << std::endl;
                delete it->second.first;
                delete it->second.second;
                _kitchenPipes.erase(it);
            }
        }
    }

    if (foundAvailable) {
        std::cerr << "Selected kitchen " << bestKitchenId << " with load "
                  << minLoad << "/" << (2 * _cookNb) << std::endl;
        return bestKitchenId;
    }

    std::cerr << "All kitchens are full/unresponsive, creating a new one..."
              << std::endl;
    std::lock_guard<std::mutex> lock(_kitchensMutex);
    unsigned int newId = _nextKitchenId++;
    createKitchen(newId);
    return newId;
}

void plazza::Reception::collectResults() {
    while (_running) {
        std::unordered_map<unsigned int, std::pair<NamedPipe *, NamedPipe *>>
            kitchensCopy;

        {
            std::lock_guard<std::mutex> lock(_kitchensMutex);
            kitchensCopy = _kitchenPipes;
        }

        std::vector<unsigned int> kitchensToRemove;

        for (auto &pair : kitchensCopy) {
            unsigned int kitchenId = pair.first;

            {
                std::lock_guard<std::mutex> lock(_kitchensMutex);
                for (size_t i = 0; i < _kitchenPids.size(); ++i) {
                    int status;
                    pid_t result = waitpid(_kitchenPids[i], &status, WNOHANG);

                    if (result > 0) {
                        kitchensToRemove.push_back(kitchenId);
                        std::cerr << "Kitchen " << kitchenId << " has closed."
                                  << std::endl;
                        break;
                    }
                }
            }
        }

        if (!kitchensToRemove.empty()) {
            std::lock_guard<std::mutex> lock(_kitchensMutex);
            for (unsigned int kitchenId : kitchensToRemove) {
                auto it = _kitchenPipes.find(kitchenId);
                if (it != _kitchenPipes.end()) {
                    delete it->second.first;
                    delete it->second.second;
                    _kitchenPipes.erase(it);
                }

                _kitchenPids.erase(
                    std::remove_if(_kitchenPids.begin(), _kitchenPids.end(),
                        [kitchenId](pid_t pid) {
                            int status;
                            return waitpid(pid, &status, WNOHANG) > 0;
                        }),
                    _kitchenPids.end());
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(500));
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
