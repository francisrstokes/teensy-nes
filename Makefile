SRC_DIR := src
OBJ_DIR := obj
BIN_DIR := bin
INC_DIR := .pio/build/teensy_hid_device/FrameworkArduino
EXE := $(BIN_DIR)/main

SRC := $(wildcard $(SRC_DIR)/*.cpp)
OBJ := $(SRC:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

CC=g++
CFLAGS=-c -Wall -I$(INC_DIR)
LDFLAGS=-Llib

all: $(EXE)

.PHONY: all

$(EXE): $(OBJ) | $(BIN_DIR)
	$(CC) $(LDFLAGS) $^ -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BIN_DIR) $(OBJ_DIR):
	mkdir -p $@

# main: test.o Bus.o CPU.o
# 	$(CC) src/test.o src/Bus.o src/CPU.o -o main

# test.o: test.cpp
# 	$(CC) $(CFLAGS) -c src/test.cpp

# Bus.o: Bus.cpp
# 	$(CC) $(CFLAGS) -c src/Bus.cpp

# CPU.o: CPU.cpp
# 	$(CC) $(CFLAGS) -c src/CPU.cpp

# %.o: *.cpp
# 	$(CC) -o $@ $< $(CFLAGS)

clean:
	@$(RM) -rv $(BIN_DIR) $(OBJ_DIR)

