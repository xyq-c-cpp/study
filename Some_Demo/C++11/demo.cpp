// demo.cpp: 定义应用程序的入口点。
//

#include <iostream>
#include <functional>
#include <chrono>
#include <ctime>
#include <memory>
#include <sys/types.h>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <exception>
#include <string>
#include <unordered_map>
#include <map>
#include <vector>
#include <cstring>
#include <future>
#include <time.h>
#include <random>
#include <regex>

class demo;

std::mutex g_lock;
std::condition_variable g_condition;
std::function<int(void)> g_func;
std::unordered_map<std::string, std::vector<std::function<int(std::string)>>> dict;
std::unordered_map<int, std::shared_ptr<demo>> g_map;

class demo : public std::enable_shared_from_this<demo> {
public:
  demo() = default;
  ~demo() {
    std::cout << __FUNCTION__ << std::endl;
  }

  int doTest() {
    time_t t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    
    char str[64];
    strftime(str, sizeof(str), "%Y-%m-%d %H:%M:%S", std::localtime(&t));
    std::cout << str << " " << __FUNCTION__ << std::endl;

    std::function<int(void)> f;
    f.swap(g_func);

    //error : bad_weak_ptr异常，此时当前对象已经被销毁。
    g_func = std::bind(&demo::doTest, shared_from_this());

    if (g_func) {
      std::cout << "g_func is callable!" << std::endl;
    }
    else {
      std::cout << "g_func is not callable!" << std::endl;
    }
    if (f) {
      std::cout << "f is callable!" << std::endl;
    }
    else {
      std::cout << "f is not callable!" << std::endl;
    }

    std::function<int(void)> ff = g_func;

    return 0;
  }

  int sub_heat_topic(std::string str) {
    std::cout << __FUNCTION__ << " " << str << std::endl;
    return 0;
  }

  std::shared_ptr<demo> self() {
    return shared_from_this();
  }
};

int subscribe(std::string topic, std::function<int(std::string)> f) {
  if (dict.find(topic) == dict.end()) {
    std::vector<std::function<int(std::string)>> tmp;
    tmp.push_back(f);
    dict[topic] = tmp;
  } else {
    dict[topic].push_back(f);
  }
  return 0;
}

int publish(std::string topic, std::string content) {
  std::cout << "publish " << topic << " begin." << std::endl;

  for (auto& i : dict[topic]) {
    i(content);
  }
  std::cout << "publish " << topic << " finish." << std::endl;
  return 0;
}

int sub_topic_heat(std::string str) {
  std::cout << __FUNCTION__ << " " << str << std::endl;
  return 0;
}

int sub_topic_heat2(std::string str) {
  std::cout << __FUNCTION__ << " " << str << std::endl;
  return 0;
}

int unordered_map_insert_thread() {
  std::thread::id id = std::this_thread::get_id();
  unsigned int tid = *(unsigned int*)&id;
  while (true) {
    for (int i = 0; i < 100; ++i) {
      bool ret;
      {
        std::unique_lock<std::mutex> lock(g_lock);
        auto iter = g_map.insert(std::make_pair(i, std::make_shared<demo>()));
        ret = iter.second;
      }
      std::cout << "thread " << tid << " insert " << i << " " 
        << (ret ? "success" : "failure") << std::endl;
    }
    //std::micro std::nano 微秒 纳秒
    std::this_thread::sleep_for(std::chrono::duration<long long, std::milli>(100)); //100ms
  }
  return 0;
}

int unordered_map_erase_thread1() {
  std::thread::id id = std::this_thread::get_id();
  unsigned int tid = *(unsigned int*)&id;
  for (; ;) {
    for (int i = 0; i < 20; ++i) {
      int index = std::rand() % 100;
      bool ret;
      {
        std::unique_lock<std::mutex> lock(g_lock);
        ret = g_map.erase(index);
      }
      std::cout << "thread " << tid << " erase " << index 
        << (ret ? " success" : " failure") << std::endl;
    }
    std::this_thread::sleep_for(std::chrono::duration<long long, std::milli>(100));
  }
  return 0;
}

std::vector<int> return_value() {
  std::vector<int> v{ 1, 2, 3, 4, 5 };
  return v;
}

int doTestFuture() {
  return 99;
}

int main(int argc, char *argv[])
{
  std::shared_ptr<demo> tmp(new demo());
  std::shared_ptr<demo> ttmp(std::move(tmp));
  tmp = ttmp;
  g_func = std::bind(&demo::doTest, tmp);
  std::function<int(void)> tmp_ff = std::bind(&demo::doTest,
    tmp->shared_from_this());

  g_func();

  if (std::is_same<int, int>::value) {
    std::cout << "int is same as int!" << std::endl;
  }
  if (std::is_same<int, long>::value) {
    std::cout << "int is same as long" << std::endl;
  }
  else {
    std::cout << "int is not same as long" << std::endl;
  }

  std::string str("hello world, i am string!");
  char tt_char[16] = { 0 };
  sprintf(tt_char, " tt_char");
  std::cout << "tt_char length: " << strlen(tt_char) << std::endl;

  
  std::cout << str << " size: " << str.size() << " length" << str.length() << std::endl;
  const char* tmp_ptr = str.c_str();
  std::cout << tmp_ptr << " c_str() length: " << strlen(tmp_ptr) << std::endl;

  str.append(tt_char);
  std::cout << str << " size: " << str.size() << " length" << str.length() << std::endl;

  std::unordered_map<int, std::string> m;
  m.insert(std::make_pair(1, std::string("xyq")));
  m.insert(std::make_pair(1, std::string("bgm")));
  m.erase(2);

  subscribe("heat", std::bind(sub_topic_heat, std::placeholders::_1));
  subscribe("heat", std::bind(sub_topic_heat2, std::placeholders::_1));
  subscribe("heat", std::bind(&demo::sub_heat_topic, tmp, std::placeholders::_1));
  publish("heat", "I am publisher!");

  std::vector<std::thread> arr;
  srand((unsigned)time(nullptr));
  arr.emplace_back(std::thread(std::bind(unordered_map_insert_thread)));
  for (int i = 0; i < 10; ++i) {
    arr.emplace_back(std::thread(std::bind(unordered_map_erase_thread1)));
  }

#if 0
  auto vv = return_value();
  for (auto& i : vv) {
    std::cout << "vv " << i << std::endl;
  }
#endif 

#if 0
  std::vector<std::future<int>> async_arr;
  async_arr.push_back(std::async(std::launch::async, unordered_map_insert_thread));
  async_arr.push_back(std::async(std::launch::async, unordered_map_erase_thread1));

  for (auto& i : async_arr) {
    std::cout << "async_arr get return " << i.get() << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(5));
  }
#endif 

#if 0
  std::future<int> fu = std::async(std::launch::async, doTestFuture);
  std::cout << "doTestFuture return value: " << fu.get() << std::endl;

  std::promise<int> promise_;
  std::future<int> ff = promise_.get_future();
  promise_.set_value(100);
  std::cout << "promise value: " << ff.get() << std::endl;

  std::packaged_task<int(void)> pack(doTestFuture);
  std::future<int> ff_ = pack.get_future();
  std::thread tt_(std::move(pack));
  std::cout << "packaged_task task future return value: " << ff_.get() << std::endl;
#endif

#if 0
  std::regex pattern(std::string("^IH\\d{4}"));
  std::regex pp(std::string("^IC\\d{4}"));
  bool ret = std::regex_match("IH2109", pattern);
  bool rr = std::regex_match("ICK001", pp);
  std::cout << "regex match result: " << ret << std::endl;
  std::cout << "regex match result: " << rr << std::endl;
#endif

#if 0
  std::default_random_engine random(time(nullptr));
  std::uniform_int_distribution<int> int_dis(0, 100); // 整数均匀分布
  std::uniform_real_distribution<float> real_dis(0.0, 1.0); // 浮点数均匀分布

  for (int i = 0; i < 10; ++i) {
    std::cout << int_dis(random) << ' ';
  }
  std::cout << std::endl;

  for (int i = 0; i < 10; ++i) {
    std::cout << real_dis(random) << ' ';
  }
  std::cout << std::endl;
#endif

  while (true) {
    std::cout << "main loop wait for 5 second..." << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(5));
  }

  return 0;
}
