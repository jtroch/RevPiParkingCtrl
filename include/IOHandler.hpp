#include <piControlIf.hpp>
#include <piControl.h>
#include <stdio.h>

class IOHandler {
    private:
        static IOHandler * instance;
        IOHandler() {};
        piControl piCtrl;

    public:
        static IOHandler * getInstance();
        int GetIO(const char* name);
        int SetIO(const char* name, uint32_t value);
};  