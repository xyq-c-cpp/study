/*
 * author: 657023321@qq.com
 * date: 2020-02-19
 * function: the main function, the entry of program.
 */
/*
 * date: 2021-07-18
 * note: restructure with C++11 and code specification.
 */

#include <webserver.h>

int main(int argc, char *argv[]) {
  threadpool* pool = new threadpool(3);
  
  webserver* mywebsvr = webserver::createwebsvr(8080, pool);

  bool rett = mywebsvr->init();
  if (!ret) {
    return -1;
  }
  
  mywebsvr->start();

  return 0;
}
