
#include "rpcDatabase.h"

//------------------------------------------------------
// Proc
//------------------------------------------------------
Proc::Proc(string& name, int* argTypes)
  :myName(name), myArgTypes(argTypes) {}

//------------------------------------------------------
// ServerLocation
//------------------------------------------------------
ServerLocation::ServerLocation(string& serverId, int port)
  :myServerId(serverId), myPort(port) {}

//------------------------------------------------------
// ServerProcList
//------------------------------------------------------
ServerProcList::ServerProcList(string& serverId, int port)
  :myLocation(serverId, port) {}

void ServerProcList::add(string& name, int* argTypes) {
  Proc proc(name, argTypes);
  myProcs.push_back(proc);
}

//------------------------------------------------------
// RpcDatabase
//------------------------------------------------------
void RpcDatabase::add(ServerProcList& list) {
  myServers.push_back(list);
}

ServerLocation RpcDatabase::getProcLocation(string& name, int* argTypes) {
  string str = "";
  ServerLocation ret(str, 0);
  return  ret;
}
