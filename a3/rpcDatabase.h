#ifndef RPCDATABASE_H
#define RPCDATABASE_H

#include <string>
#include <vector>

using std::vector;
using std::string;

class Proc {
  public:
    Proc(string& name, int* argTypes);
    bool isSameSignature(string name, int* argTypes);

  private:
    string myName;
    int* myArgTypes;
};

class ServerLocation {
  public:
    ServerLocation(string& serverId, int port);
    bool isMatchingLocation(string server, int port);

  //private:
    string myServerId;
    int myPort;
};

class ServerProcList {
  public:
    ServerProcList(string& serverId, int port, int socketFd);
    void add(string& name, int* argTypes);

  //private:
    int mySocketFd;
    ServerLocation myLocation;
    vector<Proc> myProcs;
};

class RpcDatabase {
  public:
    int add(string server, int port, int socketFd, string functionName, int* argTypes);
    void remove(int socketFd);
    ServerLocation getProcLocation(string& name, int* argTypes);

  private:
    vector<ServerProcList> myServers;
};

#endif
