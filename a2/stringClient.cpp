#include <sys/types.h>
#include <sys/socket.h>
#include <iostream>
#include <string>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
using namespace std;

int main() {
  char *serverAddress = getenv("SERVER_ADDRESS");
  char *port = getenv("SERVER_PORT");
  string data;
  vector<string> buffer;
  while (true) {
    getline(cin, data);
    buffer.push_back(data);
    string blah = buffer[0];
    buffer.erase(buffer.begin());
    cout << "Server: " << blah << endl;
  }
}
