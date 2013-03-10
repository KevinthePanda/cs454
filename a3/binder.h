#ifndef BINDER_H
#define BINDER_H

#include <string>
#include "rpcDatabase.h"

using namespace std;

class Binder {
  // Members
  private:
    vector<int> myConnections;
    vector<int> myToRemove;
    RpcDatabase *rpcDatabase;
    bool shutdown;

  // Functions
  public:
    Binder();
    ~Binder();
    void start();
    void close_connections();
    void process_connection(int sock);
    void add_connection(int sock);
};

#endif
