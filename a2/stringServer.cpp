#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <string>

using namespace std;

//#include "stringServer.h"

//StringServer::StringServer() {
//}

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
  if (sa->sa_family == AF_INET) {
    return &(((struct sockaddr_in*)sa)->sin_addr);
  }

  return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

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
  int status;
  struct addrinfo hints;
  struct addrinfo* servinfo;
  struct addrinfo* p;
  char ipstr[INET6_ADDRSTRLEN];

  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;

  status = getaddrinfo(NULL, "8888", &hints, &servinfo);
  if (status != 0) {
    fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
    return 1;
  }

  p = servinfo;
  int sock = socket(p->ai_family, p->ai_socktype, p->ai_protocol);

  status = bind(sock, servinfo->ai_addr, servinfo->ai_addrlen);
  
  status = listen(sock, 5);

  cout << "server: waiting for connections.." << endl;

  struct sockaddr_storage their_addr;
  while (true) {
    socklen_t addr_size = sizeof their_addr;
    int new_sock = accept(sock, (struct sockaddr*)&their_addr, &addr_size);

    if (new_sock < 0) {
      cerr << "ERROR: while accepting connection" << endl;
      continue;
    }

    inet_ntop(their_addr.ss_family,
        get_in_addr((struct sockaddr *)&their_addr),
        ipstr, sizeof ipstr);
    printf("server: got connection from %s\n", ipstr);

    //int msg_len = 0;
    //status = recv(new_sock, &msg_len, sizeof msg_len, 0);
    //cout << "server: message length " << msg_len << endl;

    char* msg = new char[256];
    status = recv(new_sock, msg, 256, 0);
    cout << "server: message " << msg << endl;

    string str(msg);
    string result = titleCase(str);

    cout << "server: result " << result << endl;

    const char* to_send = result.c_str();
    int msg_len = strlen(to_send);
    status = send(new_sock, to_send, msg_len, 0);

  }


  //for(p = servinfo;p != NULL; p = p->ai_next) {
    //void *addr;
    //char *ipver;

    //// get the pointer to the address itself,
    //// different fields in IPv4 and IPv6:
    //if (p->ai_family == AF_INET) { // IPv4
      //struct sockaddr_in *ipv4 = (struct sockaddr_in *)p->ai_addr;
      //addr = &(ipv4->sin_addr);
      //ipver = "IPv4";
    //} else { // IPv6
      //struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)p->ai_addr;
      //addr = &(ipv6->sin6_addr);
      //ipver = "IPv6";
    //}

    //// convert the IP to a string and print it:
    //inet_ntop(p->ai_family, addr, ipstr, sizeof ipstr);
    //printf("  %s: %s\n", ipver, ipstr);
  //}

  freeaddrinfo(servinfo); // free the linked list

  return 0;
}


//int main() {
  //cout << titleCase("test SDFLK kljsdfSDfjs $a") << endl;
//}
