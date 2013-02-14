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
    if ((*dataQueue).size() > 0) {
      string data = (*dataQueue)[0];
      dataQueue->erase(dataQueue->begin());
      cout << "Server: " << data << endl;

      // send data to server
      // block on receive

      sleep(2);
    }
  }

  pthread_exit(NULL);
}

int main() {
  char *serverAddress = getenv("SERVER_ADDRESS");
  char *port = getenv("SERVER_PORT");

  if (serverAddress == NULL || port == NULL) {
    cerr << "Missing SERVER_ADDRESS or SERVER_PORT environment variable" << endl;
    exit(EXIT_FAILURE);
  }

  string data;
  vector<string> buffer;

  pthread_t thread;
  pthread_create(&thread, NULL, SendToServer, &buffer);

  while (true) {
    getline(cin, data);
    buffer.push_back(data);
  }
}
