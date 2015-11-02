LOG = -D_DEBUG
CFLAGS = -O3 -std=c++11
EXE = roller xw sequential nbody_openmp
OUTPUT = *.o $(EXE)

all: clean mp run

run:
	# ./$(TARGET) 1 10000 1000000 0.01 test1.txt 0 enable -1 -1 3 600
	# ./$(TARGET) 1 1 200 1 test2.txt 0 enable -0.3 -0.3 0.6 600
	./$(TARGET) 10 1 200 1 test3.txt 0.2 enable -0.5 -0.5 1 500
	# ./$(TARGET) 10 1 300 1 test4.txt 0.5 enable -1 -1  2.5 500


roller:
	g++ src/roller-coaster.cpp -o roller -lpthread

seq:
	g++ src/nbody-sequential.cpp -o sequential -lX11 $(LOG) $(CFLAGS)

mp:
	$(eval TARGET := nbody_openmp)
	g++ src/nbody-openmp.cpp -o $(TARGET) -lX11 -fopenmp $(LOG) $(CFLAGS)

xw:
	g++ Xwindow_example.cpp -o xw  -lpthread -lX11

clean:
	-@rm $(OUTPUT) 2>/dev/null || true
