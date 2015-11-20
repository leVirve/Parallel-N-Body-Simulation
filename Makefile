LOG = -D_DEBUG0
CFLAGS = -O3 -std=c++11
IO = -D_IO_TIME
BH = -D_BH

EXE = roller nbody_sequential nbody_openmp nbody_pthread nbody_barnes_hut
OUTPUT = $(EXE) *_log
v = bh

all: clean roller seq mp p bh

test:
	{ time -p ./$(TARGET) 10 1 200 1 test2.txt 0 disable; } 2> $(v)_log
	cat $(v)_log

run:
	./$(TARGET) 10 1 300 1 test4.txt 0.5 enable -1 -1  2.5 500


roller:
	g++ src/roller-coaster.cpp -o roller -lpthread $(CFLAGS)

seq:
	$(eval TARGET := nbody_sequential)
	g++ src/nbody-sequential.cpp src/utils.cpp -o $(TARGET) -lX11 $(LOG) $(CFLAGS)

mp:
	$(eval TARGET := nbody_openmp)
	g++ src/nbody-openmp.cpp src/utils.cpp -o $(TARGET) -lX11 -fopenmp $(LOG) $(CFLAGS)

p:
	$(eval TARGET := nbody_pthread)
	g++ src/nbody-pthread.cpp src/utils.cpp -o $(TARGET) -lX11 -lpthread $(LOG) $(CFLAGS)

bh:
	$(eval TARGET := nbody_barnes_hut)
	g++ src/nbody-barnes_hut.cpp src/utils.cpp -o $(TARGET) -lX11 -lpthread $(LOG) $(CFLAGS) $(BH) $(IO)

clean:
	-@rm $(OUTPUT) 2>/dev/null || true
