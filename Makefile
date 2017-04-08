IDIR=./include
SRCDIR=./src
ODIR=./build
LIBS=-lonposix
CC=g++
CFLAGS=-I$(IDIR)
LIBS=-lonposix

_DEPS = piControl.h piControlIf.hpp LedAnimation.hpp RevPiParkingCtrl.hpp
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = piControlIf.o LedAnimation.o RevPiParkingCtrl.o 
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: $(SRCDIR)/%.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

RevPiParkingCtrl: $(OBJ)
	g++ -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o RevPiParkingCtrl