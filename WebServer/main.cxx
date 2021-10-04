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

int main(int argc, char *argv[]) {
  handleSigpipe();

  Server *server = Server::CreateServer(8888, 8, 2000);
  server->Init();
  server->Start();

  return 0;
}
