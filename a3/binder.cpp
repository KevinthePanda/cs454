#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <algorithm>
#include <vector>
#include <string>
#include "binder.h"
#include "common.h"

using namespace std;

Binder::Binder() {
  shutdown = false;
}

void Binder::start() {
  int status;
  struct addrinfo hints;
  struct addrinfo* servinfo;
  struct addrinfo* p;

  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;

  status = getaddrinfo(NULL, "0", &hints, &servinfo);
  if (status != 0) {
    fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
    return;
  }

  p = servinfo;
  int sock = socket(p->ai_family, p->ai_socktype, p->ai_protocol);

  status = bind(sock, servinfo->ai_addr, servinfo->ai_addrlen);

  status = listen(sock, 5);

  // Get the hostname and port and print them out
  char hostname[256];
  gethostname(hostname, 256);
  cout << "BINDER_ADDRESS " << hostname << endl;

  struct sockaddr_in sin;
  socklen_t len = sizeof(sin);
  getsockname(sock, (struct sockaddr *)&sin, &len);
  cout << "BINDER_PORT " << ntohs(sin.sin_port) << endl;

  fd_set readfds;
  int n;
  struct sockaddr_storage their_addr;

  while (true) {
    // shutdown condition is met
    if (shutdown) {
      break;
    }

    // build the connection list
    FD_ZERO(&readfds);
    FD_SET(sock, &readfds);
    n = sock;
    for (vector<int>::iterator it = myConnections.begin();
        it != myConnections.end(); ++it) {
      int connection = *it;
      FD_SET(connection, &readfds);
      if (connection > n)
        n = connection;
    }
    n = n+1;

    status = select(n, &readfds, NULL, NULL, NULL);

    if (status == -1) {
      cerr << "ERROR: select failed." << endl;
    } else {
      // one or both of the descriptors have data
      if (FD_ISSET(sock, &readfds)) {
        // ready to accept
        socklen_t addr_size = sizeof their_addr;
        int new_sock = accept(sock, (struct sockaddr*)&their_addr, &addr_size);

        if (new_sock < 0) {
          cerr << "ERROR: while accepting connection" << endl;
          close(new_sock);
          continue;
        }

        // add new connection
        add_connection(new_sock);

      } else {
        // a connection is ready to send us stuff
        for (vector<int>::iterator it = myConnections.begin();
            it != myConnections.end(); ++it) {
          int connection = *it;
          if (FD_ISSET(connection, &readfds)) {
            process_connection(connection);
          }
        }
      }
    }

  }

  // free the linked list
  freeaddrinfo(servinfo);
}

void Binder::add_connection(int sock) {
  myConnections.push_back(sock);
}

void Binder::close_connections() {
  for (vector<int>::iterator it = myToRemove.begin();
      it != myToRemove.end(); ++it) {
    myConnections.erase(remove(myConnections.begin(), myConnections.end(), *it), myConnections.end());
    close(*it);
  }
  myToRemove.clear();
}

void Binder::process_connection(int sock) {
  int status;

  // receive the buffer length
  int msg_type = 0;
  status = recv(sock, &msg_type, sizeof msg_type, 0);
  if (status < 0) {
    cerr << "ERROR: receive failed" << endl;
    return;
  }
  if (status == 0) {
    // client has closed the connection
    myToRemove.push_back(sock);
    return;
  }

  switch (msg_type) {
    case TERMINATE:
      // check that the sender has the right address
      for (vector<int>::iterator it = myConnections.begin(); it != myConnections.end(); ++it) {
        int connection = *it;
        msg_type = TERMINATE;
        send(connection, &msg_type, sizeof(msg_type), 0);
      }
      close_connections();
      shutdown = true;
      break;
    case LOC_REQUEST:
      struct CLIENT_BINDER_LOC_REQUEST* res = CLIENT_BINDER_LOC_REQUEST::readMessage(sock);
      cout << res->name << endl;
      cout << res->argTypes[0] << " " << res->argTypes[1] << " " << res->argTypes[2] << endl;
      break;
  }

}

int main() {
  Binder binder;
  binder.start();
  return 0;
}
