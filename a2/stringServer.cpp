#include <sys/types.h>
#include <sys/socket.h>
#include <iostream>
#include <string>

using namespace std;

/*
#include "stringServer.h"

StringServer::StringServer() {
}
*/

string titleCase(string str) {
  for (unsigned int i = 0; i < str.length(); i++) {
    if (i == 0 || str[i-1] == ' ') {
      str[i] = toupper(str[i]);
    } else {
      str[i] = tolower(str[i]);
    }
  }
  return str;
}

int main() {
  cout << titleCase("test SDFLK kljsdfSDfjs $a") << endl;
}
