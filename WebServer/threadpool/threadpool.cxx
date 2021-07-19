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

threadpool::threadpool(int _num) : stop(false) {
  if (_num > 0 ) {
    sum_threads = _num;
  } else {
    print("the arguement _num have faults");
  }

  for (int i = 0; i< sum_threads; ++i) {
    threads.emplace_back(&threadpool::work, this);
    print("create thread : ", i);
  }
}

threadpool::~threadpool() {
  stop.store(true);
  c_tp.notify_all();

  for (auto i = threads.begin(); i != threads.end(); ++i) {
    if (i->joinable()) {
        i->join();
    }
  }
}

Task threadpool::get_one_task() {
  std::unique_lock<std::mutex> lock(this->m_tp);

  c_tp.wait(lock, [this](){
      return !(this->task.empty());
  });

  Task _task = std::move(this->task.front());
  this->task.pop();

  return _task;
}

void threadpool::work() {
  while (true) {
    if (auto _task = this->get_one_task()) {
      _task();
    } else {
      print("function work error occurred ....");
    }
  }
}

