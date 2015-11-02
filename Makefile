LOG = -D_DEBUG
CFLAGS = -O3
TARGET = roller xw sequential
OUTPUT = *.o $(TARGET)

all: clean seq

run:
	./roller 2 2 2 2

roller:
	g++ src/roller-coaster.cpp -o roller -std=c++11 -lpthread

seq:
	g++ src/nbody-sequential.cpp -o sequential -std=c++11 -lX11 -lpthread $(LOG) $(CFLAGS)

mp:
	g++ src/nbody-openmp.cpp -o nbody_openmp -std=c++11 -fopenmp $(LOG) $(CFLAGS)

xw:
	g++ Xwindow_example.cpp -o xw  -lpthread -lX11

clean:
	-@rm $(OUTPUT) 2>/dev/null || true
