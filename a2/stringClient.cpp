#include <sys/types.h>
#include <sys/socket.h>
#include <iostream>
using namespace std;

int main() {
  string data;
  while (true) {
    cin >> data;
    cout << "Server: " << data << endl;
  }
}
