#include "rpc.h"
#include "constants.h"
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
  int sock = socket(p->ai_family, p->ai_socktype, p->ai_protocol);

  status = bind(sock, servinfo->ai_addr, servinfo->ai_addrlen);
  //status = listen(sock, 5);


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
  return 0;
}

int rpcExecute() {
  return 0;
}

// Client calls this
int rpcTerminate() {
  // verfiy that the request comes from the same ip address/hostname
  // Inform all of the servers
  // Wait for servers to terminate
  // Terminate binder
  return 0;
}
