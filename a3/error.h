#include <iostream>
#include <exception>
#include <string>
using namespace std;

class RPCError: public exception
{
  public:
    RPCError(int code, char* msg);
    virtual ~RPCError() throw();
    virtual const char* what() const throw();

  private:
    int myCode;
    string myMsg;
};
