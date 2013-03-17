#ifndef CONSTANTS_H
#define CONSTANTS_H

#include "rpc.h"
#include <string>

// return types
// successful return
#define RETURN_SUCCESS 0
#define REGISTER_SUCCESS 0
#define SEND_SUCCESS 0
#define RECEIVE_SUCCESS 0

// warnings
#define SIGNATURE_ALREADY_EXISTS 1

// errors
#define RETURN_FAILURE -1
#define REGISTER_FAILURE -2
#define SEND_FAILURE -3
#define RECEIVE_FAILURE -4
#define FUNCTION_FAILURE -5
#define NO_MATCHING_SIGNATURE -6
#define INVALID_BINDER_ADDRESS -7
#define INVALID_BINDER_PORT -8

// Message Types
#define MSG_REGISTER 0
#define MSG_REGISTER_SUCCESS 1
#define MSG_REGISTER_FAILURE 2
#define MSG_LOC_REQUEST 3
#define MSG_LOC_SUCCESS 4
#define MSG_LOC_FAILURE 5
#define MSG_EXECUTE 6
#define MSG_EXECUTE_SUCCESS 7
#define MSG_EXECUTE_FAILURE 8
#define MSG_TERMINATE 9

// Constants
#define STR_LEN 256

// Util Functions
int argTypesLength(int* argTypes);
void checkStatus(int status, int errorCode, std::string msg = "");

void sendArgs(int* argTypes, void** args, int sock);
void sendArg(int argType, int length, void* arg, int sock);
void** readArgs(int* argTypes, int sock);
void* readArg(int argType, int sock);

// Data Structs
struct PROC_SKELETON {
  char* name;
  int* argTypes;
  skeleton f;
}; // Used on server

struct PROC_LOCATION {
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

  static struct SERVER_BINDER_REGISTER* readMessage(int sock);
  int sendMessage(int sock);
};

// used with REGISTER_SUCCESS
struct SERVER_BINDER_REGISTER_SUCCESS {
  // 0 for no warning
  int warningCode;

  static struct SERVER_BINDER_REGISTER_SUCCESS* readMessage(int sock);
  int sendMessage(int sock);
};

// used with REGISTER_FAILURE
struct SERVER_BINDER_REGISTER_FAILURE {
  // this code must be negative
  int failureCode;

  static struct SERVER_BINDER_REGISTER_FAILURE* readMessage(int sock);
  int sendMessage(int sock);
};

// used with LOC_REQUEST
struct CLIENT_BINDER_LOC_REQUEST {
  char *name;
  int *argTypes;

  static struct CLIENT_BINDER_LOC_REQUEST* readMessage(int sock);
  int sendMessage(int sock);
};

// used with LOC_SUCCESS
struct CLIENT_BINDER_LOC_SUCCESS {
  char* server_identifier;
  int port;

  static struct CLIENT_BINDER_LOC_SUCCESS* readMessage(int sock);
  int sendMessage(int sock);
};

// used with LOC_FAILURE
struct CLIENT_BINDER_LOC_FAILURE {
  int reasonCode;

  static struct CLIENT_BINDER_LOC_FAILURE* readMessage(int sock);
  int sendMessage(int sock);
};

// used with EXECUTE
struct CLIENT_SERVER_EXECUTE {
  char *name;
  int *argTypes;
  void** args;

  static struct CLIENT_SERVER_EXECUTE* readMessage(int sock);
  int sendMessage(int sock);
};

// used with EXECUTE_SUCCESS
struct CLIENT_SERVER_EXECUTE_SUCCESS {
  char *name;
  int *argTypes;
  void** args;

  static struct CLIENT_SERVER_EXECUTE_SUCCESS* readMessage(int sock);
  int sendMessage(int sock);
};

// used with EXECUTE_FAILURE
struct CLIENT_SERVER_EXECUTE_FAILURE {
  int reasonCode;

  static struct CLIENT_SERVER_EXECUTE_FAILURE* readMessage(int sock);
  int sendMessage(int sock);
};

struct CLIENT_BINDER_TERMINATE {
  char* hostname;

  static struct CLIENT_BINDER_TERMINATE* readMessage(int sock);
  int sendMessage(int sock);
};

#endif
