
#include <string>

using namespace std;

class StringServer {
  // Functions
  public:
    StringServer();

    void start();
    void close_connections();
    void process_connection(int sock);
    void add_connection(int sock);

  private:
    string title_case(string str);

  // Members
  private:
    vector<int> myConnections;
    vector<int> myToRemove;
};
