#include <PosixSharedQueue.h>
#include <PosixThread.h>


final class NetworkHandler: public AbstractThread {
    private:
        PosixSharedQueue<HttpMsgType>& HttpWorkQueue;

    public:
        NetworkHandler(PosixSharedQueue<HttpMsgType>& queue);
        void run();
        
}



    