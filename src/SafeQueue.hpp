/*
** EPITECH PROJECT, 2025
** bootstrap_Plazza
** File description:
** SafeQueue
*/

#include <condition_variable>
#include <mutex>
#include <queue>

#ifndef SRC_SAFEQUEUE_HPP_
#define SRC_SAFEQUEUE_HPP_

template <typename T>
class SafeQueue {
 public:
    SafeQueue() {
        std::unique_lock<std::mutex> lock(_mutex);
        while (!_queue.empty()) {
            _queue.pop();
        }
    }

    ~SafeQueue() = default;

    void push(T value) {
        std::unique_lock<std::mutex> lock(_mutex);
        _queue.push(value);
        _condVar.notify_one();
    }

    bool tryPop(T &value) {
        std::unique_lock<std::mutex> lock(_mutex);
        if (_queue.empty()) {
            return false;
        }
        value = _queue.front();
        _queue.pop();
        return true;
    }

    T pop() {
        std::unique_lock<std::mutex> lock(_mutex);
        while (_queue.empty()) {
            _condVar.wait(lock);
        }
        T value = _queue.front();
        _queue.pop();
        return value;
    }

 private:
    std::queue<T> _queue;
    std::mutex _mutex;
    std::condition_variable _condVar;
};

#endif  // SRC_SAFEQUEUE_HPP_
