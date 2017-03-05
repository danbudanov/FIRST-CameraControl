CC=g++
CFLAGS=-pthread -std=gnu++11
BUILD_DIR=build

all: build stdSocketTools.o
	$(CC)    -o main.out main.cpp $(CFLAGS) $(BUILD_DIR)/stdSocketTools.o

build:
	mkdir build

%.o: %.cpp
	$(CC) -c -o $(BUILD_DIR)/$@ $^ $(CFLAGS)

%.o: ../libs/%.cpp
	$(CC) -c -o $(BUILD_DIR)/$@ $^ $(CFLAGS)

clean:
	- rm build/*.o
	- rm *.out
