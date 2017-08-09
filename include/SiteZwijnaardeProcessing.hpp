#include <restclient-cpp/connection.h>
#include <restclient-cpp/restclient.h>

#include "CentralProcessing.hpp"
#include "Barrier.hpp"

class SiteZwijnaardeProcessing { 
    private: 
        CentralProcessing processing; 
        Barrier * barrier;

    public:
        SiteZwijnaardeProcessing() {
            barrier = new Barrier(ENTRANCE_EXIT);
        };
        ~SiteZwijnaardeProcessing() {
            delete barrier;
        };
        void start();
        void waitForTermination();
};