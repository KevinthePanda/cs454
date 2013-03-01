#ifndef CONSTANTS_H
#define CONSTANTS_H

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

// Message Structs

// used with REGISTER
struct SERVER_BINDER_REGISTER {
  int server_identifier;
  int port;
  char *name;
  char *argTypes;
};

// used with LOC_REQUEST
struct CLIENT_BINDER_LOC_REQUEST {
  char *name;
  char *argTypes;
};

// used with LOC_SUCCESS
struct CLIENT_BINDER_LOC_SUCCESS {
  char* server_identifier;
  int port;
};

// used with LOC_FAILURE
struct CLIENT_BINDER_LOC_FAILURE {
  int reasonCode;
};

// used with EXECUTE
struct CLIENT_SERVER_EXECUTE {
  char *name;
  char *argTypes;
  char *args;
};

// used with EXECUTE_SUCCESS
struct CLIENT_SERVER_EXECUTE_SUCCESS {
  char *name;
  char *argTypes;
  char *args;
};

// used with EXECUTE_FAILURE
struct CLIENT_SERVER_EXECUTE_FAILURE {
  int reasonCode;
};

#endif
