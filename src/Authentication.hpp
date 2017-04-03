#include "IOHandler.hpp"
#include <AbstractThread.hpp>
#include <PosixSharedQueue.hpp>
#include <Logger.hpp>

class Authentication : public AbstractThread {
private:
    PosixSharedQueue<HttpMsgType>& HttpWorkQueue;

    static int Id;
    static int Key;

public:
    Authentication(WorkQueue<HttpMsgType>& queue);
    void run();

    static int GetId();
    static int GetKey();
    static void Update(int id, int key);
};