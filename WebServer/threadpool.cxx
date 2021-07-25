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

ThreadPool *ThreadPool::CreatePool(int thread_nr) {
  ThreadPool *tmp = nullptr;

  if (!tmp) {
    tmp = new ThreadPool(thread_nr);
    assert(tmp != nullptr);
  }

  return tmp;
}

void ThreadPool::Insert(void_arg_task callback) {
  {
    std::lock_guard<std::mutex> local_lock(lock_);

    task_queue_.emplace(std::move(callback));
  }

  condition_variable_.notify_one();

  return;
}

ThreadPool::ThreadPool(int thread_nr) 
    : thread_nr_(thread_nr),
      stop_(false) {
  assert(thread_nr_ > 0);

  for (int i = 0; i < thread_nr; ++i) {
    work_thread_.emplace_back(&ThreadPool::Work, this);
    LOG_DEBUG("new thread %d success", i);
  }
}

ThreadPool::~ThreadPool() {
  stop_.store(true);
  condition_variable_.notify_all();

  for (auto &item : work_thread_) {
    if (item.joinable()) {
        item.join();
    }
  }
}

void_arg_task ThreadPool::GetOneTask() {
  std::unique_lock<std::mutex> lock(lock_);

  condition_variable_.wait(lock,
    [this](){
      return !(this->task_queue_.empty());
    });

  void_arg_task task = std::move(task_queue_.front());
  task_queue_.pop();

  return std::move(task);
}

void ThreadPool::Work() {
  void_arg_task task;
  int ret;

  while (stop_) {
    task = GetOneTask();
    ret = task();
    LOG_DEBUG("called users' callback, ret %d", ret);
  }
}

