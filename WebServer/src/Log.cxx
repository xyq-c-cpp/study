#include <Log.h>

#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <stdio.h>

FILE *log_file = nullptr;
static std::mutex log_lock;
static std::queue<std::string> log_queue;
static std::thread log_thread;
static std::condition_variable con;

void logging_thread() {
  std::string buf;
  std::uint32_t cnt = 0;

  while (true) {
    {
      std::unique_lock<std::mutex> lock(log_lock);
      con.wait(lock, [] { return log_queue.empty() == false; });

      buf = std::move(log_queue.front());
      log_queue.pop();
    }

    if (log_file) {
      fwrite(buf.c_str(), 1, buf.size(), log_file);
      ++cnt;
    }

    if (cnt == 20) {
      fflush(log_file);
      cnt = 0;
    }
  }
}

int log_init(std::string file) {
  log_file = fopen(file.c_str(), "a+");
  if (log_file == nullptr) {
    return -1;
  }

  log_thread = std::thread(std::bind(logging_thread));

  return 0;
}

void log(std::string &str) {
  std::cout << str;

  std::unique_lock<std::mutex> lock(log_lock);
  log_queue.push(std::move(str));
  con.notify_one();
}
