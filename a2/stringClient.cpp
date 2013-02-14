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

struct thread_data {
  int sock;
  vector<string> *buf;
};

void *SendToServer(void *threadarg) {
  struct thread_data *my_data;
  my_data = (struct thread_data *) threadarg;

  vector<string> *dataQueue = my_data->buf;

  while (true) {
    if ((*dataQueue).size() > 0) {
      string data = (*dataQueue)[0];
      (*dataQueue).erase((*dataQueue).begin());

      // send data to server
      int len = data.length() + 1;
      send(my_data->sock, &len, sizeof(len), 0);
      send(my_data->sock, data.c_str(), len, 0);

      // block on receive
      string titleCasedData;
      recv(my_data->sock, &len, sizeof(len), 0);
      char *msg = new char[len];
      recv(my_data->sock, msg, len, 0);
      cout << "Server: " << msg << endl;

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

  struct thread_data td;
  td.sock = sockfd;
  td.buf = &buffer;

  pthread_t thread;
  pthread_create(&thread, NULL, SendToServer, (void *)&td);

  while (true) {
    getline(cin, data);
    buffer.push_back(data);
  }
}
