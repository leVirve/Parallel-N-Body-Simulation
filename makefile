TARGET = roller
OUTPUT = *.o $(TARGET)

all: clean roller run

run:
	./roller 2 2 2 2

roller:
	g++ src/roller-coaster.cpp -o roller -std=c++11 -lpthread

clean:
	-@rm $(OUTPUT) 2>/dev/null || true
