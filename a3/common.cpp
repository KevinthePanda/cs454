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
  return i+1;
}

void checkStatus(int status, char* msg) {
  if (status < 0) {
    RPCError err(status, msg);
    throw err;
  }
}

int getSizeFromArgTypes(int* argTypes) {
  int total = 0;
  int length = argTypesLength(argTypes);
  for (int i = 0; i < length; i++) {
    switch ((argTypes[i] & (15 << 16)) >> 16) {
      case ARG_CHAR:
        total += sizeof(char);
        break;
      case ARG_SHORT:
        total += sizeof(short);
        break;
      case ARG_INT:
        total += sizeof(int);
        break;
      case ARG_LONG:
        total += sizeof(long);
        break;
      case ARG_DOUBLE:
        total += sizeof(double);
        break;
      case ARG_FLOAT:
        total += sizeof(float);
        break;
    }
  }
  return total;
}

//===================================================
// SERVER_BINDER_REGISTER Member Functions
//===================================================
struct SERVER_BINDER_REGISTER* SERVER_BINDER_REGISTER::readMessage(int sock) {
  int status;
  struct SERVER_BINDER_REGISTER* ret = new struct SERVER_BINDER_REGISTER();

  try {
    // receive the server identifier
    ret->server_identifier = new char[STR_LEN];
    status = recv(sock, ret->server_identifier, STR_LEN, 0);
    if (status < 0) {
      checkStatus(RECEIVE_FAILURE, "reading SERVER_BINDER_REGISTER message");
    }

    // receive the port
    status = recv(sock, &(ret->port), sizeof ret->port, 0);
    if (status < 0) {
      checkStatus(RECEIVE_FAILURE, "reading SERVER_BINDER_REGISTER message");
    }

    // receive the function name
    int len = 0;
    status = recv(sock, &len, sizeof(len), 0);
    if (status < 0) {
      checkStatus(RECEIVE_FAILURE, "reading SERVER_BINDER_REGISTER message");
    }

    ret->name = new char[len];
    status = recv(sock, ret->name, len, 0);
    if (status < 0) {
      checkStatus(RECEIVE_FAILURE, "reading SERVER_BINDER_REGISTER message");
    }

    // receive the function args
    status = recv(sock, &len, sizeof(len), 0);
    if (status < 0) {
      checkStatus(RECEIVE_FAILURE, "reading SERVER_BINDER_REGISTER message");
    }

    ret->argTypes = new int[len];
    status = recv(sock, ret->argTypes, len*sizeof(int), 0);
    if (status < 0) {
      checkStatus(RECEIVE_FAILURE, "reading SERVER_BINDER_REGISTER message");
    }
  } catch (RPCError& e) {
    delete ret;
    ret = NULL;
  }

  return ret;
}

int SERVER_BINDER_REGISTER::sendMessage(int sock) {
  int status;
  int len;

  // send the msg type
  int msg_type = MSG_REGISTER;
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

  // send the argTypes length
  len = argTypesLength(argTypes);
  status = send(sock, &len, sizeof len, 0);
  if (status < 0) {
    cerr << "ERROR: send failed" << endl;
    return REGISTER_FAILURE;
  }

  // send the argTypes
  len = len*(sizeof(int));
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
    status = recv(sock, ret->name, len, 0);
    if (status < 0) {
      checkStatus(RECEIVE_FAILURE, "reading CLIENT_BINDER_LOC_REQUEST message");
    }

    // receive the function args
    status = recv(sock, &len, sizeof(len), 0);
    if (status < 0) {
      checkStatus(RECEIVE_FAILURE, "reading CLIENT_BINDER_LOC_REQUEST message");
    }

    ret->argTypes = new int[len];
    status = recv(sock, ret->argTypes, len * sizeof(int), 0);
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
  int msg_type = MSG_LOC_REQUEST;
  status = send(sock, &msg_type, sizeof(msg_type), 0);
  if (status < 0) {
    cerr << "ERROR: send failed" << endl;
    return RETURN_FAILURE;
  }

  // send the name length
  len = strlen(name) + 1;
  status = send(sock, &len, sizeof(len), 0);
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

  // send the argsTypes length
  len = argTypesLength(argTypes);
  status = send(sock, &len, sizeof(len), 0);
  if (status < 0) {
    cerr << "ERROR: send failed" << endl;
    return RETURN_FAILURE;
  }

  // send the argTypes
  status = send(sock, argTypes, len * sizeof(int), 0);
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
    status = recv(sock, ret->server_identifier, sizeof(ret->server_identifier), 0);
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
  int status;

  // send the msg type
  int msg_type = MSG_LOC_SUCCESS;
  status = send(sock, &msg_type, sizeof(msg_type), 0);
  if (status < 0) {
    cerr << "ERROR: send failed" << endl;
    return RETURN_FAILURE;
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

  return RETURN_SUCCESS;
}

//===================================================
// CLIENT_BINDER_LOC_FAILURE Member Functions
//===================================================
struct CLIENT_BINDER_LOC_FAILURE*
CLIENT_BINDER_LOC_FAILURE::readMessage(int sock) {
  int status;
  struct CLIENT_BINDER_LOC_FAILURE* ret = new struct CLIENT_BINDER_LOC_FAILURE();

  try {
    // receive the failure code
    status = recv(sock, &(ret->reasonCode), sizeof(ret->reasonCode), 0);
    if (status < 0) {
      checkStatus(RECEIVE_FAILURE, "reading CLIENT_BINDER_LOC_SUCCESS message");
    }
  } catch (RPCError& e) {
    delete ret;
    ret = NULL;
  }

  return ret;
}

int CLIENT_BINDER_LOC_FAILURE::sendMessage(int sock) {
  int status;

  // send the msg type
  int msg_type = MSG_LOC_FAILURE;
  status = send(sock, &msg_type, sizeof(msg_type), 0);
  if (status < 0) {
    cerr << "ERROR: send failed" << endl;
    return RETURN_FAILURE;
  }

  // send the failure code
  status = send(sock, &reasonCode, sizeof(reasonCode), 0);
  if (status < 0) {
    cerr << "ERROR: send failed" << endl;
    return REGISTER_FAILURE;
  }

  return RETURN_SUCCESS;
}

//===================================================
// CLIENT_SERVER_EXECUTE Member Functions
//===================================================
struct CLIENT_SERVER_EXECUTE*
CLIENT_SERVER_EXECUTE::readMessage(int sock) {
  int status;
  struct CLIENT_SERVER_EXECUTE* ret = new struct CLIENT_SERVER_EXECUTE();

  try {
    // receive the function name
    int len = 0;
    status = recv(sock, &len, sizeof(len), 0);
    if (status < 0) {
      checkStatus(RECEIVE_FAILURE, "reading CLIENT_BINDER_LOC_REQUEST message");
    }

    ret->name = new char[len];
    status = recv(sock, ret->name, len, 0);
    if (status < 0) {
      checkStatus(RECEIVE_FAILURE, "reading CLIENT_BINDER_LOC_REQUEST message");
    }

    // receive the function argsTypes
    status = recv(sock, &len, sizeof(len), 0);
    if (status < 0) {
      checkStatus(RECEIVE_FAILURE, "reading CLIENT_BINDER_LOC_REQUEST message");
    }

    ret->argTypes = new int[len];
    status = recv(sock, ret->argTypes, len * sizeof(int), 0);
    if (status < 0) {
      checkStatus(RECEIVE_FAILURE, "reading CLIENT_BINDER_LOC_REQUEST message");
    }
    ret->args = (void **)malloc(len * sizeof(void *));

    // receive the function args
    status = recv(sock, &len, sizeof(len), 0);
    if (status < 0) {
      checkStatus(RECEIVE_FAILURE, "reading CLIENT_BINDER_LOC_REQUEST message");
    }

    status = recv(sock, ret->args, len, 0);
    if (status < 0) {
      checkStatus(RECEIVE_FAILURE, "reading CLIENT_BINDER_LOC_REQUEST message");
    }
  } catch (RPCError& e) {
    delete ret;
    ret = NULL;
  }

  return ret;
}

int CLIENT_SERVER_EXECUTE::sendMessage(int sock) {
  int status;
  int len;

  // send the msg type
  int msg_type = MSG_EXECUTE;
  status = send(sock, &msg_type, sizeof(msg_type), 0);
  if (status < 0) {
    cerr << "ERROR: send failed" << endl;
    return RETURN_FAILURE;
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

  // send the argsTypes length
  len = argTypesLength(argTypes);
  status = send(sock, &len, sizeof(len), 0);
  if (status < 0) {
    cerr << "ERROR: send failed" << endl;
    return RETURN_FAILURE;
  }

  // send the argTypes
  status = send(sock, argTypes, len * sizeof(int), 0);
  if (status < 0) {
    cerr << "ERROR: send failed" << endl;
    return RETURN_FAILURE;
  }

  // send the args length
  len = getSizeFromArgTypes(argTypes);
  status = send(sock, &len, sizeof(len), 0);
  if (status < 0) {
    cerr << "ERROR: send failed" << endl;
    return RETURN_FAILURE;
  }

  // send the args
  status = send(sock, args, len, 0);
  if (status < 0) {
    cerr << "ERROR: send failed" << endl;
    return RETURN_FAILURE;
  }

  return RETURN_SUCCESS;

}

//===================================================
// CLIENT_SERVER_EXECUTE_SUCCESS Member Functions
//===================================================
struct CLIENT_SERVER_EXECUTE_SUCCESS* CLIENT_SERVER_EXECUTE_SUCCESS::readMessage(int sock) {
  int status;
  struct CLIENT_SERVER_EXECUTE_SUCCESS* ret = new struct CLIENT_SERVER_EXECUTE_SUCCESS();

  try {
    // receive the function name
    int len = 0;
    status = recv(sock, &len, sizeof(len), 0);
    if (status < 0) {
      checkStatus(RECEIVE_FAILURE, "reading CLIENT_BINDER_LOC_REQUEST message");
    }

    ret->name = new char[len];
    status = recv(sock, ret->name, len, 0);
    if (status < 0) {
      checkStatus(RECEIVE_FAILURE, "reading CLIENT_BINDER_LOC_REQUEST message");
    }

    // receive the function argsTypes
    status = recv(sock, &len, sizeof(len), 0);
    if (status < 0) {
      checkStatus(RECEIVE_FAILURE, "reading CLIENT_BINDER_LOC_REQUEST message");
    }

    ret->argTypes = new int[len];
    status = recv(sock, ret->argTypes, len * sizeof(int), 0);
    if (status < 0) {
      checkStatus(RECEIVE_FAILURE, "reading CLIENT_BINDER_LOC_REQUEST message");
    }
    ret->args = (void **)malloc(len * sizeof(void *));

    // receive the function args
    status = recv(sock, &len, sizeof(len), 0);
    if (status < 0) {
      checkStatus(RECEIVE_FAILURE, "reading CLIENT_BINDER_LOC_REQUEST message");
    }

    status = recv(sock, ret->args, len, 0);
    if (status < 0) {
      checkStatus(RECEIVE_FAILURE, "reading CLIENT_BINDER_LOC_REQUEST message");
    }
  } catch (RPCError& e) {
    delete ret;
    ret = NULL;
  }

  return ret;
}

int CLIENT_SERVER_EXECUTE_SUCCESS::sendMessage(int sock) {
  int status;
  int len;

  // send the msg type
  int msg_type = MSG_EXECUTE_SUCCESS;
  status = send(sock, &msg_type, sizeof(msg_type), 0);
  if (status < 0) {
    cerr << "ERROR: send failed" << endl;
    return RETURN_FAILURE;
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

  // send the argsTypes length
  len = argTypesLength(argTypes);
  status = send(sock, &len, sizeof(len), 0);
  if (status < 0) {
    cerr << "ERROR: send failed" << endl;
    return RETURN_FAILURE;
  }

  // send the argTypes
  status = send(sock, argTypes, len * sizeof(int), 0);
  if (status < 0) {
    cerr << "ERROR: send failed" << endl;
    return RETURN_FAILURE;
  }

  // send the args length
  len = getSizeFromArgTypes(argTypes);
  status = send(sock, &len, sizeof(len), 0);
  if (status < 0) {
    cerr << "ERROR: send failed" << endl;
    return RETURN_FAILURE;
  }

  // send the args
  status = send(sock, args, len, 0);
  if (status < 0) {
    cerr << "ERROR: send failed" << endl;
    return RETURN_FAILURE;
  }

  return RETURN_SUCCESS;
}

//===================================================
// CLIENT_SERVER_EXECUTE_FAILURE Member Functions
//===================================================
struct CLIENT_SERVER_EXECUTE_FAILURE*
CLIENT_SERVER_EXECUTE_FAILURE::readMessage(int sock) {
  int status;
  struct CLIENT_SERVER_EXECUTE_FAILURE* ret = new struct CLIENT_SERVER_EXECUTE_FAILURE();

  try {
    status = recv(sock, &(ret->reasonCode), sizeof(ret->reasonCode), 0);
    if (status < 0) {
      checkStatus(RECEIVE_FAILURE, "reading CLIENT_SERVER_EXECUTE_FAILURE message");
    }
  } catch (RPCError& e) {
    delete ret;
    ret = NULL;
  }

  return ret;
}

int CLIENT_SERVER_EXECUTE_FAILURE::sendMessage(int sock) {
  int status;

  // send the msg type
  int msg_type = MSG_EXECUTE_FAILURE;
  status = send(sock, &msg_type, sizeof(msg_type), 0);
  if (status < 0) {
    cerr << "ERROR: send failed" << endl;
    return RETURN_FAILURE;
  }

  // send the failure reason code
  status = send(sock, &reasonCode, sizeof(reasonCode), 0);
  if (status < 0) {
    cerr << "ERROR: send failed" << endl;
    return RETURN_FAILURE;
  }

  return RETURN_SUCCESS;
}
