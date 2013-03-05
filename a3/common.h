#ifndef CONSTANTS_H
#define CONSTANTS_H

#include "rpc.h"

// return types
#define RETURN_SUCCESS 0
#define RETURN_WARNING 1
#define RETURN_FAILURE -1
#define REGISTER_SUCCESS 0
#define REGISTER_FAILURE 1

// Message Types
#define REGISTER 0
#define LOC_REQUEST 1
#define LOC_SUCCESS 2
#define LOC_FAILURE 3
#define EXECUTE 4
#define EXECUTE_SUCCESS 5
#define EXECUTE_FAILURE 6
#define TERMINATE 7

// Constants
#define STR_LEN 256

// Util Functions
int argTypesLength(int* argTypes);

// Data Structs
struct PROCEDURE_SKELETON {
  char* name;
  int* argTypes;
  skeleton f;
}; // Used on server

struct PROCEDURE_LOCATION {
  char* name;
  int* argTypes;
  char* server_identifier;
  int port;
}; // Used on binder

// Message Structs

// used with REGISTER
struct SERVER_BINDER_REGISTER {
  char* server_identifier;
  int port;
  char* name;
  int *argTypes;

  int sendMessage(int sock);
};

// used with LOC_REQUEST
struct CLIENT_BINDER_LOC_REQUEST {
  char *name;
  int *argTypes;

  int sendMessage(int sock);
};

// used with LOC_SUCCESS
struct CLIENT_BINDER_LOC_SUCCESS {
  char* server_identifier;
  int port;

  int sendMessage(int sock);
};

// used with LOC_FAILURE
struct CLIENT_BINDER_LOC_FAILURE {
  int reasonCode;

  int sendMessage(int sock);
};

// used with EXECUTE
struct CLIENT_SERVER_EXECUTE {
  char *name;
  char *argTypes;
  char *args;

  int sendMessage(int sock);
};

// used with EXECUTE_SUCCESS
struct CLIENT_SERVER_EXECUTE_SUCCESS {
  char *name;
  char *argTypes;
  char *args;

  int sendMessage(int sock);
};

// used with EXECUTE_FAILURE
struct CLIENT_SERVER_EXECUTE_FAILURE {
  int reasonCode;

  int sendMessage(int sock);
};

#endif
