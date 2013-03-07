#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string.h>

#include "common.h"
#include "error.h"

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
  return i;
}

void checkStatus(int status, char* msg) {
  if (status < 0) {
    RPCError err(status, msg);
    throw err;
  }
}

//===================================================
// SERVER_BINDER_REGISTER Member Functions
//===================================================
int SERVER_BINDER_REGISTER::sendMessage(int sock) {
  int status;
  int len;

  // send the msg type
  int msg_type = REGISTER;
  status = send(sock, &msg_type, sizeof msg_type, 0);
  if (status < 0) {
    cerr << "ERROR: send failed" << endl;
    return REGISTER_FAILURE;
  }

  // send the server identifier
  status = send(sock, server_identifier, STR_LEN, 0);
  if (status < 0) {
    cerr << "ERROR: send failed" << endl;
    return REGISTER_FAILURE;
  }

  // send the port
  status = send(sock, &port, sizeof port, 0);
  if (status < 0) {
    cerr << "ERROR: send failed" << endl;
    return REGISTER_FAILURE;
  }

  // send the name length
  len = strlen(name) + 1;
  status = send(sock, &len, sizeof len, 0);
  if (status < 0) {
    cerr << "ERROR: send failed" << endl;
    return REGISTER_FAILURE;
  }

  // send the name
  status = send(sock, name, len, 0);
  if (status < 0) {
    cerr << "ERROR: send failed" << endl;
    return REGISTER_FAILURE;
  }

  // send the argTypes
  len = argTypesLength(argTypes)*(sizeof(int));
  status = send(sock, argTypes, len, 0);
  if (status < 0) {
    cerr << "ERROR: send failed" << endl;
    return REGISTER_FAILURE;
  }

  return REGISTER_SUCCESS;
}

//===================================================
// CLIENT_BINDER_LOC_REQUEST Member Functions
//===================================================
struct CLIENT_BINDER_LOC_REQUEST* CLIENT_BINDER_LOC_REQUEST::readMessage(int sock) {
  int status;
  struct CLIENT_BINDER_LOC_REQUEST* ret = new struct CLIENT_BINDER_LOC_REQUEST();

  try {
    // receive the function name
    int len = 0;
    status = recv(sock, &len, sizeof(len), 0);
    if (status < 0) {
      checkStatus(RECEIVE_FAILURE, "reading CLIENT_BINDER_LOC_REQUEST message");
    }

    ret->name = new char[len];
    status = recv(sock, ret->name, sizeof(ret->name), 0);
    if (status < 0) {
      checkStatus(RECEIVE_FAILURE, "reading CLIENT_BINDER_LOC_REQUEST message");
    }

    // receive the function args
    len = 0;
    status = recv(sock, &len, sizeof(len), 0);
    if (status < 0) {
      checkStatus(RECEIVE_FAILURE, "reading CLIENT_BINDER_LOC_REQUEST message");
    }

    ret->argTypes = new int[len];
    status = recv(sock, ret->argTypes, len, 0);
    if (status < 0) {
      checkStatus(RECEIVE_FAILURE, "reading CLIENT_BINDER_LOC_REQUEST message");
    }
  } catch (RPCError& e) {
    delete ret;
    ret = NULL;
  }

  return ret;
}

int CLIENT_BINDER_LOC_REQUEST::sendMessage(int sock) {
  int status;
  int len;

  // send the msg type
  int msg_type = LOC_REQUEST;
  status = send(sock, &msg_type, sizeof msg_type, 0);
  if (status < 0) {
    cerr << "ERROR: send failed" << endl;
    return RETURN_FAILURE;
  }

  // send the name length
  len = strlen(name) + 1;
  status = send(sock, &len, sizeof len, 0);
  if (status < 0) {
    cerr << "ERROR: send failed" << endl;
    return RETURN_FAILURE;
  }

  // send the name
  status = send(sock, name, len, 0);
  if (status < 0) {
    cerr << "ERROR: send failed" << endl;
    return RETURN_FAILURE;
  }

  // send the argTypes
  len = argTypesLength(argTypes)*(sizeof(int));
  status = send(sock, argTypes, len, 0);
  if (status < 0) {
    cerr << "ERROR: send failed" << endl;
    return RETURN_FAILURE;
  }

  return RETURN_SUCCESS;
}

//===================================================
// CLIENT_BINDER_LOC_SUCCESS Member Functions
//===================================================
struct CLIENT_BINDER_LOC_SUCCESS*
CLIENT_BINDER_LOC_SUCCESS::readMessage(int sock) {
  int status;
  struct CLIENT_BINDER_LOC_SUCCESS* ret = new struct CLIENT_BINDER_LOC_SUCCESS();

  try {
    // receive the server identifier
    ret->server_identifier = new char[STR_LEN];
    status = recv(sock, ret->server_identifier, STR_LEN, 0);
    if (status < 0) {
      checkStatus(RECEIVE_FAILURE, "reading CLIENT_BINDER_LOC_SUCCESS message");
    }

    // receive the port
    status = recv(sock, &(ret->port), sizeof ret->port, 0);
    if (status < 0) {
      checkStatus(RECEIVE_FAILURE, "reading CLIENT_BINDER_LOC_SUCCESS message");
    }
  } catch (RPCError& e) {
    delete ret;
    ret = NULL;
  }

  return ret;
}

int CLIENT_BINDER_LOC_SUCCESS::sendMessage(int sock) {
  return RETURN_SUCCESS;
}

//===================================================
// CLIENT_BINDER_LOC_FAILURE Member Functions
//===================================================
int CLIENT_BINDER_LOC_FAILURE::sendMessage(int sock) {
  return RETURN_SUCCESS;
}

//===================================================
// CLIENT_SERVER_EXECUTE Member Functions
//===================================================
int CLIENT_SERVER_EXECUTE::sendMessage(int sock) {
  return RETURN_SUCCESS;
}

//===================================================
// CLIENT_SERVER_EXECUTE_SUCCESS Member Functions
//===================================================
struct CLIENT_SERVER_EXECUTE_SUCCESS*
CLIENT_SERVER_EXECUTE_SUCCESS::readMessage(int sock) {
  int status;
  struct CLIENT_SERVER_EXECUTE_SUCCESS* ret = new struct CLIENT_SERVER_EXECUTE_SUCCESS();

  try {
    // receive the name length
    // receive the name
    // receive the arg types
    // receive the args

    //status = recv(sock, &(ret->port), sizeof ret->port, 0);
    //if (status < 0) {
      //checkStatus(RECEIVE_FAILURE, "reading CLIENT_BINDER_LOC_SUCCESS message");
    //}
  } catch (RPCError& e) {
    delete ret;
    ret = NULL;
  }

  return ret;
}
int CLIENT_SERVER_EXECUTE_SUCCESS::sendMessage(int sock) {
  return RETURN_SUCCESS;
}

//===================================================
// CLIENT_SERVER_EXECUTE_FAILURE Member Functions
//===================================================
int CLIENT_SERVER_EXECUTE_FAILURE::sendMessage(int sock) {
  return RETURN_SUCCESS;
}
