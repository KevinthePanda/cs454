#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

#include "stringServer.h"

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa) {
  if (sa->sa_family == AF_INET) {
    return &(((struct sockaddr_in*)sa)->sin_addr);
  }

  return &(((struct sockaddr_in6*)sa)->sin6_addr);
}


StringServer::StringServer() {
}


string StringServer::title_case(string str) {
  for (unsigned int i = 0; i < str.length(); i++) {
    if (i == 0 || str[i-1] == ' ') {
      str[i] = toupper(str[i]);
    } else {
      str[i] = tolower(str[i]);
    }
  }
  return str;
}

void StringServer::start() {
  int status;
  struct addrinfo hints;
  struct addrinfo* servinfo;
  struct addrinfo* p;
  char ipstr[INET6_ADDRSTRLEN];

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
  cout << "SERVER ADDRESS " << hostname << endl;

  struct sockaddr_in sin;
  socklen_t len = sizeof(sin);
  getsockname(sock, (struct sockaddr *)&sin, &len);
  cout << "SERVER_PORT " << ntohs(sin.sin_port) << endl;

  fd_set readfds;
  int n;
  struct sockaddr_storage their_addr;


  while (true) {
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
        cout << "num connections: " << myConnections.size() << endl;

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
      close_connections();
    }

  }

  // free the linked list
  freeaddrinfo(servinfo);
}

void StringServer::add_connection(int sock) {
  myConnections.push_back(sock);
}

void StringServer::close_connections() {
  for (vector<int>::iterator it = myToRemove.begin();
      it != myToRemove.end(); ++it) {
    myConnections.erase(remove(myConnections.begin(), myConnections.end(), *it), myConnections.end());
    close(*it);
  }
  myToRemove.clear();
}

void StringServer::process_connection(int sock) {
  int status;

  // receive the buffer length
  int msg_len = 0;
  status = recv(sock, &msg_len, sizeof msg_len, 0);
  if (status < 0) {
    cerr << "ERROR: receive failed" << endl;
    return;
  }
  if (status == 0) {
    // client has closed the connection
    cout << "server: client " << sock << " closed connection" << endl;
    myToRemove.push_back(sock);
    return;
  }
  cout << "server: message length - " << msg_len << endl;

  // receive the string
  char* msg = new char[msg_len];
  status = recv(sock, msg, msg_len, 0);
  if (status < 0) {
    cerr << "ERROR: receive failed" << endl;
    return;
  }
  cout << "server: message - " << msg << endl;

  // title case the string
  string str(msg);
  string result = title_case(str);
  cout << "server: result - " << result << endl;

  // send the buffer length
  const char* to_send = result.c_str();
  msg_len = strlen(to_send) + 1;
  status = send(sock, &msg_len, sizeof msg_len, 0);
  if (status < 0) {
    cerr << "ERROR: send failed" << endl;
    return;
  }

  // send the result string
  status = send(sock, to_send, msg_len, 0);
  if (status < 0) {
    cerr << "ERROR: send failed" << endl;
    return;
  }

  delete[] msg;
}

