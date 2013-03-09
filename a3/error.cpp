#include <stdlib.h>

#include "error.h"
#include "common.h"

RPCError::RPCError(int code, string msg)
  :myCode(code), myMsg(msg) {}

RPCError::~RPCError() throw() {}

const char* RPCError::what() const throw() {
  string ret;
  switch(myCode) {
    case SEND_FAILURE:
      ret = "Send Failure (";
      break;
    case RECEIVE_FAILURE:
      ret = "Receive Failure (";
      break;
    default:
      ret = "Failure (";
      break;
  }

  ret += myCode;
  ret += ") : ";
  ret += myMsg;

  return ret.c_str();
}
