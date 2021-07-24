/*
 * author: 657023321@qq.com
 * date: 2020-02-19
 * function: the defination of the pool of thread
 */
/*
 * date: 2021-07-18
 * note: restructure with C++11 and code specification.
 */

#include "threadpool.h"
#include "common.h"
#include <future>
#include <stdexcept>

using Task = std::function<void()>;

ThreadPool::CreatePool(int thread_nr) {
  ThreadPool *tmp = nullptr;

  if (!tmp) {
    tmp = new ThreadPool(thread_nr);
    static_assert(tmp != nullptr, "new threadpool failed");
  }

  return tmp;
}

void ThreadPool::Insert(std::function<int()> &callback) {
  {
    std::lock_guard<std::mutex> local_lock(lock_);

    task_queue_.emplace(std::move(callback));
  }

  condition_variable_.notify_one();

  return;
}

ThreadPool::ThreadPool(int thread_nr) 
    : thread_nr(thread_nr),
      stop(std::atomic<bool>(false)) {
  static_assert(thread_nr > 0, "thread number should be greater than zero");

  for (int i = 0; i < thread_nr; ++i) {
    threads.emplace_back(&ThreadPool::Work, this);
    LOG_DEBUG("new thread %d success", i);
  }
}

ThreadPool::ThreadPool() {
  stop_.store(true);
  condition_variable_.notify_all();

  for (auto item : work_thread_) {
    if (item.joinable()) {
        item.join();
    }
  }
}

std::function<int()>& ThreadPool::GetOneTask() {
  std::unique_lock<std::mutex> lock(this->lock);

  condition_variable_.wait(lock,
    [this](){
      return !(this->task_queue_.empty());
    });

  std::function<int()> task = std::move(task_queue_.front());
  task_queue_.pop();

  return std::move(task);
}

void threadpool::Work() {
  std::function<int()> func;
  int ret;

  while (stop_) {
    func = GetOneTask();
    ret = func();
    LOG_DEBUG("called users' callback, ret %d", ret);
  }
}

