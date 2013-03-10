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
    ServerProcList(string& serverId, int port);
    void add(string& name, int* argTypes);

  //private:
    ServerLocation myLocation;
    vector<Proc> myProcs;
};

class RpcDatabase {
  public:
    //void add(ServerProcList& list);
    void add(string server, int port, string functionName, int* argTypes);
    ServerLocation getProcLocation(string& name, int* argTypes);

  private:
    vector<ServerProcList> myServers;
};

#endif
