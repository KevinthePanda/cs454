#include "rpc.h"
#include "common.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <iostream>
#include <algorithm>
#include <vector>
#include <pthread.h>

using std::cerr;
using std::cout;
using std::endl;
using std::vector;
using std::remove;

// Global variables
int my_binder_sock;

char* my_server_identifier;
int my_server_port;
int my_server_sock;
vector<int> my_server_connections;
vector<int> my_server_to_remove;

vector<struct PROCEDURE_SKELETON> my_server_procedures;

// server calls this
int rpcInit() {
  char *binderAddress = getenv("BINDER_ADDRESS");
  char *binderPort = getenv("BINDER_PORT");

  // open connection to binder
  int sockfd, portno;
  struct sockaddr_in binder_addr;
  struct hostent *binder;
  portno = atoi(binderPort);
  sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  binder = gethostbyname(binderAddress);

  bzero((char *) &binder_addr, sizeof(binder_addr));
  binder_addr.sin_family = AF_INET;
  bcopy((char *)binder->h_addr,
        (char *)&binder_addr.sin_addr.s_addr,
        binder->h_length);
  binder_addr.sin_port = htons(portno);

  connect(sockfd,(struct sockaddr *)&binder_addr, sizeof(binder_addr));
  my_binder_sock = sockfd;

  // create a socket for clients to connect to
  int status;
  struct addrinfo hints;
  struct addrinfo* servinfo;
  struct addrinfo* p;

  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;

  status = getaddrinfo(NULL, "0", &hints, &servinfo);
  /*
  if (status != 0) {
    fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
    return;
  }
  */

  p = servinfo;
  my_server_sock = socket(p->ai_family, p->ai_socktype, p->ai_protocol);

  status = bind(my_server_sock, servinfo->ai_addr, servinfo->ai_addrlen);

  status = listen(my_server_sock, 5);
  // get server identifier
  my_server_identifier = new char[STR_LEN];
  gethostname(my_server_identifier, STR_LEN);
  cout << "server " << my_server_identifier << endl;

  // get port number
  struct sockaddr_in sin;
  socklen_t len = sizeof(sin);
  getsockname(my_server_sock, (struct sockaddr *)&sin, &len);
  my_server_port = ntohs(sin.sin_port);
  cout << "port " << my_server_port << endl;

  //status = listen(my_server_sock, 5);


  return RETURN_SUCCESS;
}

int rpcCall(char* name, int* argTypes, void** args) {
  char *binderAddress = getenv("BINDER_ADDRESS");
  char *binderPort = getenv("BINDER_PORT");

  // make call to binder
  // socket stuff to binder
  struct CLIENT_BINDER_LOC_REQUEST req;

  // if (returnMessage == CLIENT_BINDER_LOC_FAILURE) {
  //   return RETURN_FAILURE;
  // }
  return RETURN_SUCCESS;
}

int rpcCacheCall(char* name, int* argTypes, void** args) {
  return 0;
}

int rpcRegister(char* name, int* argTypes, skeleton f) {
  int len;
  int status;

  // make an entry in local database
  struct PROCEDURE_SKELETON procedure;
  // copy the function name so we own it
  len = strlen(name) + 1;
  procedure.name = new char[len];
  strncpy(procedure.name, name, len);
  // copy the argTypes so we own it
  len = argTypesLength(argTypes);
  procedure.argTypes = new int[len];
  memcpy(procedure.argTypes, argTypes, len*sizeof(int));
  procedure.f = f;
  my_server_procedures.push_back(procedure);

  // send the message to binder
  struct SERVER_BINDER_REGISTER msg;
  msg.server_identifier = my_server_identifier;
  msg.port = my_server_port;
  msg.name = procedure.name;
  msg.argTypes = procedure.argTypes;

  status = msg.sendMessage(my_binder_sock);

  if (status < 0) {
    return status;
  }

  return REGISTER_SUCCESS;
}

int rpcExecute() {
  fd_set readfds;
  int n;
  struct sockaddr_storage their_addr;

  // start listening
  int status = listen(my_server_sock, 5);

  while (true) {
    // build the connection list
    FD_ZERO(&readfds);
    FD_SET(my_server_sock, &readfds);
    n = my_server_sock;
    for (vector<int>::iterator it = my_server_connections.begin();
        it != my_server_connections.end(); ++it) {
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
      if (FD_ISSET(my_server_sock, &readfds)) {
        // ready to accept
        socklen_t addr_size = sizeof their_addr;
        int new_sock = accept(my_server_sock, (struct sockaddr*)&their_addr, &addr_size);

        if (new_sock < 0) {
          cerr << "ERROR: while accepting connection" << endl;
          close(new_sock);
          continue;
        }

        // add new connection
        my_server_connections.push_back(my_server_sock);

      } else {
        // a connection is ready to send us stuff
        for (vector<int>::iterator it = my_server_connections.begin();
            it != my_server_connections.end(); ++it) {
          int connection = *it;
          if (FD_ISSET(connection, &readfds)) {
            //process_server_message(connection);

            int status;

            // receive the message type
            int msg_type = 0;
            status = recv(connection, &msg_type, sizeof msg_type, 0);

            if (status < 0) {
              cerr << "ERROR: receive failed" << endl;
              return -1;
            }
            if (status == 0) {
              // client has closed the connection
              my_server_to_remove.push_back(connection);
              return -1;
            }

            switch (msg_type) {
              case TERMINATE:
                return 0;
                break;
            }
          }
        }
      }
      // close connections
      for (vector<int>::iterator it = my_server_to_remove.begin();
          it != my_server_to_remove.end(); ++it) {
        my_server_connections.erase(remove(my_server_connections.begin(), my_server_connections.end(), *it), my_server_connections.end());
        close(*it);
      }
      my_server_to_remove.clear();
    }

  }
  return 0;
}

// Client calls this
int rpcTerminate() {
  // verify that the request comes from the same ip address/hostname
  // Inform all of the servers
  // Wait for servers to terminate
  // Terminate binder
  return 0;
}
