#include <sys/types.h>
#include <sys/socket.h>
#include <iostream>
#include <string>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
using namespace std;

void *SendToServer(void *threadarg) {
  vector<string> *dataQueue = (vector<string> *) threadarg;

  while (true) {
    if (*dataQueue.size() > 0) {
      string data = (*dataQueue)[0];
      dataQueue->erase(dataQueue->begin());

      // send data to server
      // block on receive

      sleep(2000);
    }
  }

  pthread_exit(NULL);
}

int main() {
  char *serverAddress = getenv("SERVER_ADDRESS");
  char *port = getenv("SERVER_PORT");
  string data;
  vector<string> buffer;
  buffer.push_back("tester");

  pthread_t thread;
  pthread_create(&thread, NULL, SendToServer, &buffer);

  while (true) {
    getline(cin, data);
    buffer.push_back(data);
    string blah = buffer[0];
    buffer.erase(buffer.begin());
    cout << "Server: " << blah << endl;
  }
}
