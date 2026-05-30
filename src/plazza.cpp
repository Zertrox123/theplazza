/*
** EPITECH PROJECT, 2026
** 2nd_year
** File description:
** plazza
*/

#include "Plazza.hpp"

void PlazzaMutex::lock()
{
    _mutex.lock(); 
}

void PlazzaMutex::unlock()
{
    _mutex.unlock();
}

std::mutex& PlazzaMutex::get()
{
    return _mutex;
}

void PlazzaCondVar::notifyOne()
{
    _cv.notify_one();
}

void PlazzaCondVar::notifyAll()
{
    _cv.notify_all();
}

void PlazzaCondVar::wait(std::unique_lock<std::mutex>& lock, std::function<bool()> pred)
{
    _cv.wait(lock, pred);
}

void Queue::push(const Order& item)
{
    _mutex.lock();
    _queue.push(item);
    _mutex.unlock();
}