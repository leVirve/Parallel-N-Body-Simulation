LOG = -D_DEBUG0
CFLAGS = -O3 -std=c++11
EXE = roller xw sequential nbody_openmp nbody_barnes_hut
OUTPUT = *.o $(EXE)
v = bh

all: clean $(v) test

test:
	{ time -p ./$(TARGET) 10 1 200 1 test2.txt 0 disable -0.3 -0.3 0.6 600; } 2> $(v)_log
	cat $(v)_log

run:
	# ./$(TARGET) 1 10000 1000000 0.01 test1.txt 0 enable -1 -1 3 600
	# ./$(TARGET) 10 1 200 1 test2.txt 0 enable -0.3 -0.3 0.6 600
	# ./$(TARGET) 10 1 200 1 test3.txt 0.2 enable -0.5 -0.5 1 500
	# ./$(TARGET) 10 1 300 1 test4.txt 0.5 enable -1 -1  2.5 500


roller:
	g++ src/roller-coaster.cpp -o roller -lpthread $(CFLAGS)

seq:
	$(eval TARGET := sequential)
	g++ src/nbody-sequential.cpp src/utils.cpp -o $(TARGET) -lX11 $(LOG) $(CFLAGS)

mp:
	$(eval TARGET := nbody_openmp)
	g++ src/nbody-openmp.cpp src/utils.cpp -o $(TARGET) -lX11 -fopenmp $(LOG) $(CFLAGS)

p:
	$(eval TARGET := nbody_pthread)
	g++ src/nbody-pthread.cpp src/utils.cpp -o $(TARGET) -lX11 -lpthread $(LOG) $(CFLAGS)

bh:
	$(eval TARGET := nbody_barnes_hut)
	g++ src/nbody-barnes_hut.cpp src/utils.cpp -o $(TARGET) -lX11 -lpthread $(LOG) $(CFLAGS)

xw:
	g++ Xwindow_example.cpp -o xw  -lpthread -lX11

clean:
	-@rm $(OUTPUT) 2>/dev/null || true
