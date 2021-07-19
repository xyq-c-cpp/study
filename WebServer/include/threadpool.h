/*
 * author: 657023321@qq.com
 * date: 2020-02-19
 * function: the defination of the pool of thread
 */
/*
 * date: 2021-07-18
 * note: restructure with C++11 and code specification.
 */

#ifndef _THREAD_POOL_H_
#define _THREAD_POOL_H_

#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <queue>
#include <functional>
#include <atomic>
#include <future>

class ThreadPool
{
 public:
  ThreadPool(uint32_t thread_nr);
  ~ThreadPool();

  template<class F, class... ARGS>
  auto push(F&& f, ARGS&&... args)->std::future<decltype(f(args...))> {
    if(stop.load()) {
      exit(-1);
    }

    using rettype = decltype(f(args...));
    auto _task = std::make_shared<std::packaged_task<rettype()>>(
      std::bind(std::forward<F>(f), std::forward<ARGS>(args)...)
      );

    std::lock_guard<std::mutex> lock(this->m_tp);
    this->task.emplace([_task](){
      (*_task)();
      }
    );

    this->c_tp.notify_one();
    std::future<rettype> future = _task->get_future();

    return future;
  }

 private:
  void Work(void);
  Task GetOneTask();

  using Task = std::function<void()>;

  std::mutex lock_;
  std::condition_variable condition_variable_;

  std::vector<std::thread> work_thread_;
  uint32_t thread_nr_;
  std::queue<Task> task_queue_;

  std::atomic<bool> stop_;
};

#endif /* _THREAD_POOL_H_ */