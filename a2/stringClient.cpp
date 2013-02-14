#include <sys/types.h>
#include <sys/socket.h>
#include <iostream>
#include <vector>
using namespace std;

int main() {
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
