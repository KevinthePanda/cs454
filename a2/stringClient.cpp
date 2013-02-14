#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <iostream>
#include <string>
#include <vector>
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

  int sockfd, portno, n;
  struct sockaddr_in serv_addr;
  struct hostent *server;
  portno = atoi(port);
  sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  server = gethostbyname(serverAddress);

  bzero((char *) &serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  bcopy((char *)server->h_addr,
        (char *)&serv_addr.sin_addr.s_addr,
        server->h_length);
  serv_addr.sin_port = htons(portno);

  connect(sockfd,(struct sockaddr *)&serv_addr, sizeof(serv_addr));


  printf("SUCCESS !!! Connection established \n");

  while (true) {
    getline(cin, data);
    buffer.push_back(data);
  }
}
