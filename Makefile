CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -O2 -g -D_DEFAULT_SOURCE -I./src/components/HUD -I./src/components/adt

TARGET = src/municipal_system
TEST_EXE = src/test/unit_test
TEST_SRC = src/test/main.t.c

OBJ_DIR = src/components/object
OBJS = $(OBJ_DIR)/main.o $(OBJ_DIR)/hud.o $(OBJ_DIR)/struct.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

$(OBJ_DIR)/main.o: src/main.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c src/main.c -o $(OBJ_DIR)/main.o

$(OBJ_DIR)/hud.o: src/components/HUD/hud.c src/components/HUD/hud.h src/components/adt/struct.h
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c src/components/HUD/hud.c -o $(OBJ_DIR)/hud.o

$(OBJ_DIR)/struct.o: src/components/adt/struct.c src/components/adt/struct.h
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c src/components/adt/struct.c -o $(OBJ_DIR)/struct.o


test: $(OBJ_DIR)/struct.o
	@mkdir -p src/test
	$(CC) $(CFLAGS) $(TEST_SRC) $(OBJ_DIR)/struct.o -o $(TEST_EXE)
	cd src && valgrind --leak-check=full --show-leak-kinds=all ./test/unit_test
	@rm -f $(TEST_EXE)

valgrind: $(TARGET)
	cd src && valgrind --leak-check=full --show-leak-kinds=all ./municipal_system


run: $(TARGET)
	cd src && ./municipal_system

debug: $(TARGET)
	cd src && gdb ./municipal_system

clean:
	rm -f $(TARGET) $(OBJS)
	rm -rf $(OBJ_DIR)
	rm -f $(TEST_EXE)

.PHONY: all clean run valgrind debug test
