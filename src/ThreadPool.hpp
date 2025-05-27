/*
** EPITECH PROJECT, 2025
** ThreadPool.hpp
** File description:
** _
*/

#ifndef SRC_PLAZZA_KITCHEN_THREADPOOL_HPP_
#define SRC_PLAZZA_KITCHEN_THREADPOOL_HPP_

#include <atomic>
#include <condition_variable>
#include <functional>
#include <future>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

namespace plazza {
namespace kitchen {

class ThreadPool {
 public:
    ThreadPool(size_t numThreads);
    ~ThreadPool();

    template <class F, class... Args>
    auto enqueue(F &&f, Args &&...args)
        -> std::future<typename std::result_of<F(Args...)>::type>;

    size_t getBusyCount() const;

    size_t getThreadCount() const;

 private:
    std::vector<std::thread> _workers;

    std::queue<std::function<void()>> _tasks;

    mutable std::mutex _queueMutex;
    std::condition_variable _condition;

    std::atomic<bool> _stop;
    std::atomic<size_t> _busyCount;
};

template <class F, class... Args>
auto ThreadPool::enqueue(F &&f, Args &&...args)
    -> std::future<typename std::result_of<F(Args...)>::type> {
    using return_type = typename std::result_of<F(Args...)>::type;

    auto task = std::make_shared<std::packaged_task<return_type()>>(
        std::bind(std::forward<F>(f), std::forward<Args>(args)...));

    std::future<return_type> res = task->get_future();

    {
        std::unique_lock<std::mutex> lock(_queueMutex);

        if (_stop) {
            throw std::runtime_error("enqueue on stopped ThreadPool");
        }

        _tasks.emplace([task, this]() {
            _busyCount++;
            (*task)();
            _busyCount--;
        });
    }

    _condition.notify_one();
    return res;
}

}  // namespace kitchen
}  // namespace plazza

#endif  // SRC_PLAZZA_KITCHEN_THREADPOOL_HPP_
