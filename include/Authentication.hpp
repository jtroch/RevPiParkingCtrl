#include <onposix/AbstractThread.hpp>
#include <onposix/PosixSharedQueue.hpp>

#include <restclient-cpp/connection.h>
#include <restclient-cpp/restclient.h>

using namespace onposix;

class Authentication : public AbstractThread {
private:
    RestClient::Connection* HttpConnection;

    static int Id;
    static int Key;
    
public:
    Authentication();
    void run();

    static int GetId();
    static int GetKey();
    static void Update(int id, int key);
};