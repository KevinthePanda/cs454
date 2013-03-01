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

int rpcInit() {
  return 0;
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
