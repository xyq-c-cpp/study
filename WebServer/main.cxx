/*
 * author: 657023321@qq.com
 * date: 2020-02-19
 * function: the main function, the entry of program.
 */
/*
 * date: 2021-07-18
 * note: restructure with C++11 and code specification.
 */

#include <common.h>
#include <server.h>
#include <signal.h>

int main(int argc, char *argv[]) {
  struct sigaction act;
  memset(&act, 0, sizeof(act));
  act.sa_handler = SIG_IGN;
  (void)sigaction(SIGPIPE, &act, nullptr);
  
  Server *server = Server::CreateServer(8888, 8, 2000);

  server->Init();
  server->Start();

  return 0;
}
