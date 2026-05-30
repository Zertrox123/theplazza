/*
** EPITECH PROJECT, 2026
** 2nd_year
** File description:
** Plazza
*/

#ifndef PLAZZA_HPP_
#define PLAZZA_HPP_

#pragma once
#include <mutex>
#include <condition_variable>
#include <queue>
#include "ipc.hpp"

class PlazzaMutex {
    public:
        PlazzaMutex() = default;
        ~PlazzaMutex() = default;
        void lock();
        void unlock();
        std::mutex& get();
    private:
        std::mutex _mutex;
};

class PlazzaCondVar {
    public:
        PlazzaCondVar() = default;
        ~PlazzaCondVar() = default;
        void notifyOne();
        void notifyAll();
        template<typename Lock, typename Predicate>
        void wait(Lock& lock, Predicate pred);
    private:
        std::condition_variable _cv;
};

class Queue {
    public:
        SafeQueue() = default;
        ~SafeQueue() = default;
        void push(const Order& item);
        bool tryPop(Order& item) {
            _mutex.lock();
            if (_queue.empty()) {
                _mutex.unlock();
                return false;
            }
            item = _queue.front();
            _queue.pop();
            _mutex.unlock();
            return true;
        }
        bool isEmpty() {
            _mutex.lock();
            bool empty = _queue.empty();
            _mutex.unlock();
            return empty;
        }
    private:
        std::queue<Order> _queue;
        PlazzaMutex _mutex;
};

#endif /* !PLAZZA_HPP_ */
