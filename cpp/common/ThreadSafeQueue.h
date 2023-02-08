// https://www.geeksforgeeks.org/implement-thread-safe-queue-in-c/

#include <condition_variable>
#include <iostream>
#include <mutex>
#include <queue>

#pragma once

template<class Data>
struct ThreadSafeQueue {

  // Push ====================================================================================================

  void push(Data item) {
    std::unique_lock<std::mutex> lock(this->mutex);

    this->queue.push(item);

    this->cond.notify_one();
  }

  // Pop =====================================================================================================

  inline Data pop() {
    std::unique_lock<std::mutex> lock(this->mutex);

    this->cond.wait(lock, [this]() { return !this->queue.empty(); });

    Data item = this->queue.front();
    this->queue.pop();

    return item;
  }

 private:
  // Member Variables ========================================================================================
  std::queue<Data>        queue;
  std::mutex              mutex;
  std::condition_variable cond;
};
