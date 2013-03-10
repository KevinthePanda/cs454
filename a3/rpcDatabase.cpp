#include "rpcDatabase.h"
#include "common.h"

//------------------------------------------------------
// Proc
//------------------------------------------------------
Proc::Proc(string& name, int* argTypes)
  :myName(name), myArgTypes(argTypes) {}

bool Proc::isSameSignature(string name, int* argTypes) {
  if (name.compare(myName) != 0) {
    return false;
  }
  int len = argTypesLength(argTypes);
  if (len != argTypesLength(myArgTypes)) {
    return false;
  }

  for (int i = 0; i < len; i++) {
    if (argTypes[i] != myArgTypes[i]) {
      return false;
    }
  }
  return true;
}

//------------------------------------------------------
// ServerLocation
//------------------------------------------------------
ServerLocation::ServerLocation(string& serverId, int port)
  :myServerId(serverId), myPort(port) {}

bool ServerLocation::isMatchingLocation(string server, int port) {
  if (server.compare(myServerId) == 0 && port == myPort) {
    return true;
  }
  return false;
}

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
void RpcDatabase::add(string server, int port, string functionName, int* argTypes) {
  // search for a server
  for (unsigned int i = 0; i < myServers.size(); i++) {
    if (myServers[i].myLocation.isMatchingLocation(server, port)) {
      myServers[i].add(functionName, argTypes);
      return;
    }
  }

  // not found, add the server to the list
  ServerProcList procList(server, port);
  procList.add(functionName, argTypes);
  myServers.push_back(procList);
}

ServerLocation RpcDatabase::getProcLocation(string& name, int* argTypes) {
  string str = "";
  ServerLocation ret(str, 0);
  bool found = false;
  int loc = -1;;

  // search for a server
  for (unsigned int i = 0; i < myServers.size() && !found; i++) {
    for (unsigned int j = 0; j < myServers[i].myProcs.size() && !found; j++) {
      if (myServers[i].myProcs[j].isSameSignature(name, argTypes)) {
        found = true;
        ret = myServers[i].myLocation;
        loc = i;
        break;
      }
    }
  }

  // rearrange the list
  if (loc > 0) {
    ServerProcList procList = myServers[loc];
    myServers.erase(myServers.begin() + loc);
    myServers.push_back(procList);
  }

  return ret;
}
