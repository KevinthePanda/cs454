#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <string.h>

#include "common.h"
#include "error.h"

using std::string;
using std::cerr;
using std::cout;
using std::endl;

//===================================================
// Util Functions
//===================================================
int argTypesLength(int* argTypes) {
  int i = 0;
  int elem = argTypes[0];
  while (elem != 0) {
    i++;
    elem = argTypes[i];
  }
  return i+1;
}

void checkStatus(int status, int errorCode, string msg) {
  if (status < 0) {
    RPCError err(errorCode, msg);
    throw err;
  }
}

void sendArgs(int* argTypes, void** args, int sock) {
  int length = argTypesLength(argTypes) - 1;
  for (int i = 0; i < length; i++) {
    int argType = (argTypes[i] & (15 << 16)) >> 16;
    int argLength = (argTypes[i] & ((1 << 16) - 1));
    void* arg = args[i];
    sendArg(argType, argLength, arg, sock);
  }
}

void sendArg(int argType, int length, void* arg, int sock) {
  int size;
  int status;
  string errorMsg = "sending arg";

  if (length == 0)
    length = 1;

  switch (argType) {
    case ARG_CHAR:
      size = sizeof(char);
      break;
    case ARG_SHORT:
      size = sizeof(short);
      break;
    case ARG_INT:
      size = sizeof(int);
      break;
    case ARG_LONG:
      size = sizeof(long);
      break;
    case ARG_DOUBLE:
      size = sizeof(double);
      break;
    case ARG_FLOAT:
      size = sizeof(float);
      break;
  }

  size = size*length;

  try {
    // send the arg size
    status = send(sock, &size, sizeof(size), 0);
    checkStatus(status, SEND_FAILURE, errorMsg);

    // send the arg
    status = send(sock, arg, size, 0);
    checkStatus(status, SEND_FAILURE, errorMsg);
  } catch (RPCError& e) {
    cerr << e.what() << endl;
  }
}

void** readArgs(int* argTypes, int sock) {
  int length = argTypesLength(argTypes) - 1;
  //void** args = (void **)malloc(length * sizeof(void *));
  void** args = new void*[length];
  for (int i = 0; i < length; i++) {
    int argType = (argTypes[i] & (15 << 16)) >> 16;
    void* arg = readArg(argType, sock);
    args[i] = arg;
  }
  return args;
}

void* readArg(int argType, int sock) {
  int size;
  int status;
  string errorMsg = "receiving arg";

  try {
    // read the arg size
    status = recv(sock, &size, sizeof(size), 0);
    checkStatus(status, RECEIVE_FAILURE, errorMsg);

    void* arg = (void *)malloc(size);
    // read the arg
    status = recv(sock, arg, size, 0);
    checkStatus(status, RECEIVE_FAILURE, errorMsg);

    return arg;
  } catch (RPCError& e) {
    cerr << e.what() << endl;
  }
  return NULL;
}

//===================================================
// SERVER_BINDER_REGISTER Member Functions
//===================================================
struct SERVER_BINDER_REGISTER* SERVER_BINDER_REGISTER::readMessage(int sock) {
  int status, len;
  struct SERVER_BINDER_REGISTER* ret = new struct SERVER_BINDER_REGISTER();
  string errorMsg = "reading SERVER_BINDER_REGISTER message";

  try {
    // receive the server identifier
    ret->server_identifier = new char[STR_LEN];
    status = recv(sock, ret->server_identifier, STR_LEN, 0);
    checkStatus(status, RECEIVE_FAILURE, errorMsg);

    // receive the port
    status = recv(sock, &(ret->port), sizeof(ret->port), 0);
    checkStatus(status, RECEIVE_FAILURE, errorMsg);

    // receive the function name
    status = recv(sock, &len, sizeof(len), 0);
    checkStatus(status, RECEIVE_FAILURE, errorMsg);

    ret->name = new char[len];
    status = recv(sock, ret->name, len, 0);
    checkStatus(status, RECEIVE_FAILURE, errorMsg);

    // receive the function args
    status = recv(sock, &len, sizeof(len), 0);
    checkStatus(status, RECEIVE_FAILURE, errorMsg);

    ret->argTypes = new int[len];
    status = recv(sock, ret->argTypes, len * sizeof(int), 0);
    checkStatus(status, RECEIVE_FAILURE, errorMsg);
  } catch (RPCError& e) {
    delete ret;
    ret = NULL;
  }

  return ret;
}

int SERVER_BINDER_REGISTER::sendMessage(int sock) {
  int status, len;
  int msg_type = MSG_REGISTER;
  string errorMsg = "sending SERVER_BINDER_REGISTER message";

  try {
    // send the msg type
    status = send(sock, &msg_type, sizeof(msg_type), 0);
    checkStatus(status, SEND_FAILURE, errorMsg);

    // send the server identifier
    status = send(sock, server_identifier, STR_LEN, 0);
    checkStatus(status, SEND_FAILURE, errorMsg);

    // send the port
    status = send(sock, &port, sizeof(port), 0);
    checkStatus(status, SEND_FAILURE, errorMsg);

    // send the name length
    len = strlen(name) + 1;
    status = send(sock, &len, sizeof(len), 0);
    checkStatus(status, SEND_FAILURE, errorMsg);

    // send the name
    status = send(sock, name, len, 0);
    checkStatus(status, SEND_FAILURE, errorMsg);

    // send the argTypes length
    len = argTypesLength(argTypes);
    status = send(sock, &len, sizeof(len), 0);
    checkStatus(status, SEND_FAILURE, errorMsg);

    // send the argTypes
    len = len*(sizeof(int));
    status = send(sock, argTypes, len, 0);
    checkStatus(status, SEND_FAILURE, errorMsg);
  } catch (RPCError& e) {
    return SEND_FAILURE;
  }

  return SEND_SUCCESS;
}

//===================================================
// SERVER_BINDER_REGISTER_SUCCESS Member Functions
//===================================================
struct SERVER_BINDER_REGISTER_SUCCESS* SERVER_BINDER_REGISTER_SUCCESS::readMessage(int sock) {
  int status;
  struct SERVER_BINDER_REGISTER_SUCCESS* ret = new struct SERVER_BINDER_REGISTER_SUCCESS();
  string errorMsg = "reading SERVER_BINDER_REGISTER_SUCCESS message";

  try {
    // receive the warning code
    status = recv(sock, &(ret->warningCode), sizeof(ret->warningCode), 0);
    checkStatus(status, RECEIVE_FAILURE, errorMsg);
  } catch (RPCError& e) {
    delete ret;
    ret = NULL;
  }

  return ret;
}

int SERVER_BINDER_REGISTER_SUCCESS::sendMessage(int sock) {
  int status;
  int msg_type = MSG_REGISTER_SUCCESS;
  string errorMsg = "sending SERVER_BINDER_REGISTER_SUCCESS message";

  try {
    // send the msg type
    status = send(sock, &msg_type, sizeof(msg_type), 0);
    checkStatus(status, SEND_FAILURE, errorMsg);

    // send the warning code
    status = send(sock, &warningCode, sizeof(warningCode), 0);
    checkStatus(status, SEND_FAILURE, errorMsg);
  } catch (RPCError& e) {
    return SEND_FAILURE;
  }

  return SEND_SUCCESS;
}

//===================================================
// SERVER_BINDER_REGISTER_FAILURE Member Functions
//===================================================
struct SERVER_BINDER_REGISTER_FAILURE* SERVER_BINDER_REGISTER_FAILURE::readMessage(int sock) {
  int status;
  struct SERVER_BINDER_REGISTER_FAILURE* ret = new struct SERVER_BINDER_REGISTER_FAILURE();
  string errorMsg = "reading SERVER_BINDER_REGISTER_FAILURE message";

  try {
    // receive the warning code
    status = recv(sock, &(ret->failureCode), sizeof(ret->failureCode), 0);
    checkStatus(status, RECEIVE_FAILURE, errorMsg);
  } catch (RPCError& e) {
    delete ret;
    ret = NULL;
  }

  return ret;
}

int SERVER_BINDER_REGISTER_FAILURE::sendMessage(int sock) {
  int status;
  int msg_type = MSG_REGISTER_FAILURE;
  string errorMsg = "sending SERVER_BINDER_REGISTER_FAILURE message";

  try {
    // send the msg type
    status = send(sock, &msg_type, sizeof(msg_type), 0);
    checkStatus(status, SEND_FAILURE, errorMsg);

    // send the warning code
    status = send(sock, &failureCode, sizeof(failureCode), 0);
    checkStatus(status, SEND_FAILURE, errorMsg);
  } catch (RPCError& e) {
    return SEND_FAILURE;
  }

  return SEND_SUCCESS;
}

//===================================================
// CLIENT_BINDER_LOC_REQUEST Member Functions
//===================================================
struct CLIENT_BINDER_LOC_REQUEST* CLIENT_BINDER_LOC_REQUEST::readMessage(int sock) {
  int status;
  struct CLIENT_BINDER_LOC_REQUEST* ret = new struct CLIENT_BINDER_LOC_REQUEST();
  string errorMsg = "reading CLIENT_BINDER_LOC_REQUEST message";

  try {
    // receive the function name
    int len = 0;
    status = recv(sock, &len, sizeof(len), 0);
    checkStatus(status, RECEIVE_FAILURE, errorMsg);

    ret->name = new char[len];
    status = recv(sock, ret->name, len, 0);
    checkStatus(status, RECEIVE_FAILURE, errorMsg);

    // receive the function args
    status = recv(sock, &len, sizeof(len), 0);
    checkStatus(status, RECEIVE_FAILURE, errorMsg);

    ret->argTypes = new int[len];
    status = recv(sock, ret->argTypes, len * sizeof(int), 0);
    checkStatus(status, RECEIVE_FAILURE, errorMsg);
  } catch (RPCError& e) {
    delete ret;
    ret = NULL;
  }

  return ret;
}

int CLIENT_BINDER_LOC_REQUEST::sendMessage(int sock) {
  int status, len;
  int msg_type = MSG_LOC_REQUEST;
  string errorMsg = "sending CLIENT_BINDER_LOC_REQUEST message";

  try {
    // send the msg type
    status = send(sock, &msg_type, sizeof(msg_type), 0);
    checkStatus(status, SEND_FAILURE, errorMsg);

    // send the name length
    len = strlen(name) + 1;
    status = send(sock, &len, sizeof(len), 0);
    checkStatus(status, SEND_FAILURE, errorMsg);

    // send the name
    status = send(sock, name, len, 0);
    checkStatus(status, SEND_FAILURE, errorMsg);

    // send the argsTypes length
    len = argTypesLength(argTypes);
    status = send(sock, &len, sizeof(len), 0);
    checkStatus(status, SEND_FAILURE, errorMsg);

    // send the argTypes
    status = send(sock, argTypes, len * sizeof(int), 0);
    checkStatus(status, SEND_FAILURE, errorMsg);
  } catch (RPCError& e) {
    return SEND_FAILURE;
  }

  return SEND_SUCCESS;
}

//===================================================
// CLIENT_BINDER_LOC_SUCCESS Member Functions
//===================================================
struct CLIENT_BINDER_LOC_SUCCESS*
CLIENT_BINDER_LOC_SUCCESS::readMessage(int sock) {
  int status;
  struct CLIENT_BINDER_LOC_SUCCESS* ret = new struct CLIENT_BINDER_LOC_SUCCESS();
  string errorMsg = "reading CLIENT_BINDER_LOC_SUCCESS message";

  try {
    // receive the server identifier
    ret->server_identifier = new char[STR_LEN];
    status = recv(sock, ret->server_identifier, STR_LEN, 0);
    checkStatus(status, RECEIVE_FAILURE, errorMsg);

    // receive the port
    status = recv(sock, &(ret->port), sizeof(ret->port), 0);
    checkStatus(status, RECEIVE_FAILURE, errorMsg);
  } catch (RPCError& e) {
    delete ret;
    ret = NULL;
  }

  return ret;
}

int CLIENT_BINDER_LOC_SUCCESS::sendMessage(int sock) {
  int status;
  int msg_type = MSG_LOC_SUCCESS;
  string errorMsg = "sending CLIENT_BINDER_LOC_SUCCESS message";

  try {
    // send the msg type
    status = send(sock, &msg_type, sizeof(msg_type), 0);
    checkStatus(status, SEND_FAILURE, errorMsg);

    // send the server identifier
    status = send(sock, server_identifier, STR_LEN, 0);
    checkStatus(status, SEND_FAILURE, errorMsg);

    // send the port
    status = send(sock, &port, sizeof(port), 0);
    checkStatus(status, SEND_FAILURE, errorMsg);
  } catch (RPCError& e) {
    return SEND_FAILURE;
  }

  return SEND_SUCCESS;
}

//===================================================
// CLIENT_BINDER_LOC_FAILURE Member Functions
//===================================================
struct CLIENT_BINDER_LOC_FAILURE*
CLIENT_BINDER_LOC_FAILURE::readMessage(int sock) {
  int status;
  struct CLIENT_BINDER_LOC_FAILURE* ret = new struct CLIENT_BINDER_LOC_FAILURE();
  string errorMsg = "reading CLIENT_BINDER_LOC_FAILURE message";

  try {
    // receive the failure code
    status = recv(sock, &(ret->reasonCode), sizeof(ret->reasonCode), 0);
    checkStatus(status, RECEIVE_FAILURE, errorMsg);
  } catch (RPCError& e) {
    delete ret;
    ret = NULL;
  }

  return ret;
}

int CLIENT_BINDER_LOC_FAILURE::sendMessage(int sock) {
  int status;
  int msg_type = MSG_LOC_FAILURE;
  string errorMsg = "sending CLIENT_BINDER_LOC_FAILURE message";

  try {
    // send the msg type
    status = send(sock, &msg_type, sizeof(msg_type), 0);
    checkStatus(status, SEND_FAILURE, errorMsg);

    // send the failure code
    status = send(sock, &reasonCode, sizeof(reasonCode), 0);
    checkStatus(status, SEND_FAILURE, errorMsg);
  } catch (RPCError& e) {
    return SEND_FAILURE;
  }

  return SEND_SUCCESS;
}

//===================================================
// CLIENT_SERVER_EXECUTE Member Functions
//===================================================
struct CLIENT_SERVER_EXECUTE*
CLIENT_SERVER_EXECUTE::readMessage(int sock) {
  int status, len;
  struct CLIENT_SERVER_EXECUTE* ret = new struct CLIENT_SERVER_EXECUTE();
  string errorMsg = "reading CLIENT_SERVER_EXECUTE message";

  try {
    // receive the function name
    status = recv(sock, &len, sizeof(len), 0);
    checkStatus(status, RECEIVE_FAILURE, errorMsg);

    ret->name = new char[len];
    status = recv(sock, ret->name, len, 0);
    checkStatus(status, RECEIVE_FAILURE, errorMsg);

    // receive the function argsTypes
    status = recv(sock, &len, sizeof(len), 0);
    checkStatus(status, RECEIVE_FAILURE, errorMsg);

    ret->argTypes = new int[len];
    status = recv(sock, ret->argTypes, len * sizeof(int), 0);
    checkStatus(status, RECEIVE_FAILURE, errorMsg);
    ret->args = readArgs(ret->argTypes, sock);
    /*
    ret->args = (void **)malloc(len * sizeof(void *));

    // receive the function args
    status = recv(sock, &len, sizeof(len), 0);
    checkStatus(status, RECEIVE_FAILURE, errorMsg);

    status = recv(sock, ret->args, len, 0);
    checkStatus(status, RECEIVE_FAILURE, errorMsg);
    */
  } catch (RPCError& e) {
    delete ret;
    ret = NULL;
  }

  return ret;
}

int CLIENT_SERVER_EXECUTE::sendMessage(int sock) {
  int status, len;
  int msg_type = MSG_EXECUTE;
  string errorMsg = "sending CLIENT_SERVER_EXECUTE message";

  try {
    // send the msg type
    status = send(sock, &msg_type, sizeof(msg_type), 0);
    checkStatus(status, SEND_FAILURE, errorMsg);

    // send the name length
    len = strlen(name) + 1;
    status = send(sock, &len, sizeof(len), 0);
    checkStatus(status, SEND_FAILURE, errorMsg);

    // send the name
    status = send(sock, name, len, 0);
    checkStatus(status, SEND_FAILURE, errorMsg);

    // send the argsTypes length
    len = argTypesLength(argTypes);
    status = send(sock, &len, sizeof(len), 0);
    checkStatus(status, SEND_FAILURE, errorMsg);

    // send the argTypes
    status = send(sock, argTypes, len * sizeof(int), 0);
    checkStatus(status, SEND_FAILURE, errorMsg);

    // send the args length
    sendArgs(argTypes, args, sock);
    /*
    len = getSizeFromArgTypes(argTypes);
    status = send(sock, &len, sizeof(len), 0);
    checkStatus(status, SEND_FAILURE, errorMsg);

    // send the args
    status = send(sock, args, len, 0);
    checkStatus(status, SEND_FAILURE, errorMsg);
    */
  } catch (RPCError& e) {
    return SEND_FAILURE;
  }

  return SEND_SUCCESS;
}

//===================================================
// CLIENT_SERVER_EXECUTE_SUCCESS Member Functions
//===================================================
struct CLIENT_SERVER_EXECUTE_SUCCESS* CLIENT_SERVER_EXECUTE_SUCCESS::readMessage(int sock) {
  int status;
  struct CLIENT_SERVER_EXECUTE_SUCCESS* ret = new struct CLIENT_SERVER_EXECUTE_SUCCESS();
  string errorMsg = "reading CLIENT_SERVER_EXECUTE_SUCCESS message";

  try {
    // receive the function name
    int len = 0;
    status = recv(sock, &len, sizeof(len), 0);
    checkStatus(status, RECEIVE_FAILURE, errorMsg);

    ret->name = new char[len];
    status = recv(sock, ret->name, len, 0);
    checkStatus(status, RECEIVE_FAILURE, errorMsg);

    // receive the function argsTypes
    status = recv(sock, &len, sizeof(len), 0);
    checkStatus(status, RECEIVE_FAILURE, errorMsg);

    ret->argTypes = new int[len];
    status = recv(sock, ret->argTypes, len * sizeof(int), 0);
    checkStatus(status, RECEIVE_FAILURE, errorMsg);
    ret->args = readArgs(ret->argTypes, sock);
  } catch (RPCError& e) {
    delete ret;
    ret = NULL;
  }

  return ret;
}

int CLIENT_SERVER_EXECUTE_SUCCESS::sendMessage(int sock) {
  int status, len;
  int msg_type = MSG_EXECUTE_SUCCESS;
  string errorMsg = "sending CLIENT_SERVER_EXECUTE_SUCCESS message";

  try {
    // send the msg type
    status = send(sock, &msg_type, sizeof(msg_type), 0);
    checkStatus(status, SEND_FAILURE, errorMsg);

    // send the name length
    len = strlen(name) + 1;
    status = send(sock, &len, sizeof(len), 0);
    checkStatus(status, SEND_FAILURE, errorMsg);

    // send the name
    status = send(sock, name, len, 0);
    checkStatus(status, SEND_FAILURE, errorMsg);

    // send the argsTypes length
    len = argTypesLength(argTypes);
    status = send(sock, &len, sizeof(len), 0);
    checkStatus(status, SEND_FAILURE, errorMsg);

    // send the argTypes
    status = send(sock, argTypes, len * sizeof(int), 0);
    checkStatus(status, SEND_FAILURE, errorMsg);

    // send the args
    sendArgs(argTypes, args, sock);
  } catch (RPCError& e) {
    return SEND_FAILURE;
  }

  return SEND_SUCCESS;
}

//===================================================
// CLIENT_SERVER_EXECUTE_FAILURE Member Functions
//===================================================
struct CLIENT_SERVER_EXECUTE_FAILURE*
CLIENT_SERVER_EXECUTE_FAILURE::readMessage(int sock) {
  int status;
  struct CLIENT_SERVER_EXECUTE_FAILURE* ret = new struct CLIENT_SERVER_EXECUTE_FAILURE();
  string errorMsg = "reading CLIENT_SERVER_EXECUTE_FAILURE message";

  try {
    status = recv(sock, &(ret->reasonCode), sizeof(ret->reasonCode), 0);
    checkStatus(status, RECEIVE_FAILURE, errorMsg);
  } catch (RPCError& e) {
    delete ret;
    ret = NULL;
  }

  return ret;
}

int CLIENT_SERVER_EXECUTE_FAILURE::sendMessage(int sock) {
  int status;
  int msg_type = MSG_EXECUTE_FAILURE;
  string errorMsg = "sending CLIENT_SERVER_EXECUTE_FAILURE message";

  try {
    // send the msg type
    status = send(sock, &msg_type, sizeof(msg_type), 0);
    checkStatus(status, SEND_FAILURE, errorMsg);

    // send the failure reason code
    status = send(sock, &reasonCode, sizeof(reasonCode), 0);
    checkStatus(status, SEND_FAILURE, errorMsg);
  } catch (RPCError& e) {
    return SEND_FAILURE;
  }

  return SEND_SUCCESS;
}

//===================================================
// CLIENT_SERVER_TERMINATE Member Functions
//===================================================
struct CLIENT_BINDER_TERMINATE*
CLIENT_BINDER_TERMINATE::readMessage(int sock) {
  int status;
  struct CLIENT_BINDER_TERMINATE* ret = new struct CLIENT_BINDER_TERMINATE();
  string errorMsg = "reading CLIENT_BINDER_TERMINATE message";

  try {
    ret->hostname = new char[STR_LEN];
    status = recv(sock, ret->hostname, STR_LEN, 0);
    checkStatus(status, RECEIVE_FAILURE, errorMsg);
  } catch (RPCError& e) {
    delete ret;
    ret = NULL;
  }

  return ret;
}

int CLIENT_BINDER_TERMINATE::sendMessage(int sock) {
  int status;
  int msg_type = MSG_TERMINATE;
  string errorMsg = "sending CLIENT_BINDER_TERMINATE message";

  //cerr << "terminate send" << endl;

  try {
    // send the msg type
    status = send(sock, &msg_type, sizeof(msg_type), 0);
    checkStatus(status, SEND_FAILURE, errorMsg);

    // send the host name
    status = send(sock, hostname, STR_LEN, 0);
    checkStatus(status, SEND_FAILURE, errorMsg);
  } catch (RPCError& e) {
    return SEND_FAILURE;
  }

  return SEND_SUCCESS;
}
