CC = gcc
CFLAGS = -Wall -Wextra -g -I./components/HUD -I./components/adt

TARGET = municipal_system

TEST = ./test/main.t.c

OBJ_DIR = components/object

OBJS = $(OBJ_DIR)/main.o $(OBJ_DIR)/hud.o $(OBJ_DIR)/struct.o

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

$(OBJ_DIR)/main.o: main.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c main.c -o $(OBJ_DIR)/main.o

$(OBJ_DIR)/hud.o: components/HUD/hud.c components/HUD/hud.h components/adt/struct.h
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c components/HUD/hud.c -o $(OBJ_DIR)/hud.o

$(OBJ_DIR)/struct.o: components/adt/struct.c components/adt/struct.h
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c components/adt/struct.c -o $(OBJ_DIR)/struct.o

clean:
	rm -f $(TARGET) $(OBJS)
	rm -rf $(OBJ_DIR)
	rm -f test/unit_test

run: $(TARGET)
	./$(TARGET)

valgrind: $(TARGET)
	valgrind --leak-check=full --show-leak-kinds=all ./$(TARGET)

debug: $(TARGET)
	gdb ./$(TARGET)

.PHONY: clean run valgrind debug test

test: $(OBJ_DIR)/struct.o
	$(CC) $(CFLAGS) $(TEST) $(OBJ_DIR)/struct.o -o test/unit_test
	valgrind --leak-check=full --show-leak-kinds=all ./test/unit_test
	@rm -f test/unit_test