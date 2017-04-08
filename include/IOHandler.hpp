final class IOHandler {
    private:
        static volatile IOHandler instance = null;
        IOHandler();

    public:
        static IOHandler getInstance();
        int GetIO(char* name);
        int SetIO(char* name, uint32_t value);
}



    