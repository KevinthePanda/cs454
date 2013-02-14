#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

#include "stringServer.h"

int main() {

  StringServer stringServer;
  stringServer.start();

  return 0;
}
