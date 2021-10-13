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

#include <common.h>

class ThreadPool {
 public:
  static ThreadPool *CreatePool(int thread_nr);
  void InsertTask(Task callback);
  void Init(void);
  void StopWork() {
    running_.store(false);
  }

 private:
  ThreadPool(int thread_nr);
  ~ThreadPool();
  
  void Work(void);

  const int thread_nr_;
  std::atomic<bool> running_;

  std::vector<std::thread> work_thread_;
  std::queue<Task> task_queue_;

  std::condition_variable condition_variable_;
  std::mutex lock_;
};

#endif /* _THREAD_POOL_H_ */