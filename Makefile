IDIR=./include
SRCDIR=./src
ODIR=./build
LIBS=-lrestclient-cpp -lonposix -ljsoncpp 
CC=g++
CFLAGS=-I$(IDIR) -L./libs -L/usr/local/lib

_DEPS = piControl.h piControlIf.hpp LedAnimation.hpp ThreadSynchronization.hpp Http.hpp Settings.hpp Authentication.hpp IOHandler.hpp VehicleDetection.hpp RevPiParkingCtrl.hpp
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = piControlIf.o LedAnimation.o ThreadSynchronization.o Http.o Settings.o Authentication.o IOHandler.o VehicleDetection.o RevPiParkingCtrl.o 
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: $(SRCDIR)/%.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

RevPiParkingCtrl: $(OBJ)
	g++ -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o RevPiParkingCtrl