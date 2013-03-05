#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string.h>

#include "common.h"

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
int CLIENT_BINDER_LOC_REQUEST::sendMessage(int sock) {
  return RETURN_SUCCESS;
}

//===================================================
// CLIENT_BINDER_LOC_SUCCESS Member Functions
//===================================================
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
int CLIENT_SERVER_EXECUTE_SUCCESS::sendMessage(int sock) {
  return RETURN_SUCCESS;
}

//===================================================
// CLIENT_SERVER_EXECUTE_FAILURE Member Functions
//===================================================
int CLIENT_SERVER_EXECUTE_FAILURE::sendMessage(int sock) {
  return RETURN_SUCCESS;
}
