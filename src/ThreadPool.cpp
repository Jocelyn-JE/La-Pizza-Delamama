/*
** EPITECH PROJECT, 2025
** ThreadPool.cpp
** File description:
** _
*/

#include "ThreadPool.hpp"

namespace plazza {
namespace kitchen {

ThreadPool::ThreadPool(size_t numThreads) : _stop(false), _busyCount(0) {
    for (size_t i = 0; i < numThreads; ++i) {
        _workers.emplace_back([this] {
            while (true) {
                std::function<void()> task;

                {
                    std::unique_lock<std::mutex> lock(this->_queueMutex);

                    this->_condition.wait(lock, [this] {
                        return this->_stop || !this->_tasks.empty();
                    });

                    if (this->_stop && this->_tasks.empty()) {
                        return;
                    }

                    task = std::move(this->_tasks.front());
                    this->_tasks.pop();
                }

                task();
            }
        });
    }
}

ThreadPool::~ThreadPool() {
    {
        std::unique_lock<std::mutex> lock(_queueMutex);
        _stop = true;
    }

    _condition.notify_all();

    for (std::thread &worker : _workers) {
        worker.join();
    }
}

size_t ThreadPool::getBusyCount() const {
    return _busyCount;
}

size_t ThreadPool::getThreadCount() const {
    return _workers.size();
}

}  // namespace kitchen
}  // namespace plazza
