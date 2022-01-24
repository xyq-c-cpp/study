/*
 * author: 657023321@qq.com
 * date: 2020-02-19
 * function: the main function, the entry of program.
 */
/*
 * date: 2021-07-18
 * note: restructure with C++11 and code specification.
 */

#include <Common.h>
#include <EventLoop.h>
#include <EventLoopThread.h>
#include <Server.h>
#ifndef _MSC_VER
#include <getopt.h>
#endif
#include <Log.h>

#define ENDL '\n'

int main(int argc, char *argv[]) {
  int port = 8888, threadNum = 3;
  const char *str = "t:p:";
  int opt;

  while ((opt = getopt(argc, argv, str)) != -1) {
    switch (opt) {
    case 't':
      threadNum = atoi(optarg);
      break;
    case 'p':
      port = atoi(optarg);
      break;
    default:
      break;
    }
  }

  assert(log_init("/WebServer/svr.log") == 0);

#ifdef DEBUG
  logger() << "mainThread tid " << std::this_thread::get_id();
  logger() << "EPOLLIN " << EPOLLIN << '\n'
           << "EPOLLOUT " << EPOLLOUT << ENDL << "EPOLLET " << EPOLLET << ENDL
           << "EPOLLHUP " << EPOLLHUP << ENDL << "EPOLLERR " << EPOLLERR << ENDL
           << "EPOLLIN | EPOLLET " << (EPOLLET | EPOLLIN) << ENDL
           << "EPOLLOUT | EPOLLET " << (EPOLLOUT | EPOLLET) << ENDL
           << "EPOLLIN | EPOLLET | EPOLLONESHOT "
           << (EPOLLET | EPOLLIN | EPOLLONESHOT) << ENDL
           << "EPOLLOUT | EPOLLET | EPOLLONESHOT "
           << (EPOLLOUT | EPOLLET | EPOLLONESHOT) << ENDL;
#endif

  handleSigpipe();
  std::shared_ptr<EventLoop> mainLoop(std::make_shared<EventLoop>());
  std::shared_ptr<Server> server(
      std::make_shared<Server>(port, mainLoop, threadNum));
  server->start();
  
  mainLoop->loop();

  return 0;
}
