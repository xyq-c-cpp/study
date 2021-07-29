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
  static ThreadPool *tmp = nullptr;

  if (!tmp) {
    tmp = new ThreadPool(thread_nr);
    assert(tmp != nullptr);
  }

  return tmp;
}

void ThreadPool::InsertTask(void_arg_task callback) {
  {
    std::lock_guard<std::mutex> local_lock(lock_);

    task_queue_.emplace(std::move(callback));

    condition_variable_.notify_one();
  }

  return;
}

ThreadPool::ThreadPool(int thread_nr) 
    : thread_nr_(thread_nr),
      running_(true),
      work_thread_(std::vector<std::thread>(0)){
  assert(thread_nr_ > 0);

}

ThreadPool::~ThreadPool() {
  LOG_DEBUG("thread pool exit, ");
  running_.store(false);
  condition_variable_.notify_all();

  for (auto &item : work_thread_) {
    if (item.joinable()) {
        item.join();
    }
  }
}

void ThreadPool::Init(void) {
  for (int i = 0; i < thread_nr_; ++i) {
    work_thread_.emplace_back(&ThreadPool::Work, this);
    LOG_DEBUG("create thread %d success", i);
  }
}

void ThreadPool::Work() {
  void_arg_task task;
  int ret;

  while (running_) {
    {
      std::unique_lock<std::mutex> lock(lock_);
      condition_variable_.wait(lock, 
        [this](){
          return !this->task_queue_.empty() || !this->running_.load();
        }
      );
        
      if (!running_.load()) {
        LOG_DEBUG("this thread would exit, ppid %d", getppid());
        return;
      }
      
      if (task_queue_.empty()) {
        LOG_DEBUG("thread %d spurious wakeup, the task is empty, continue", getppid());
      }
      
      task = std::move(task_queue_.front());
      task_queue_.pop();
    }
  
    ret = task();
    LOG_DEBUG("called users' callback, ret %d", ret);
  }
}

