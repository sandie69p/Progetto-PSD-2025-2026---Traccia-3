CC = gcc
CFLAGS = -Wall -Wextra -g -I./components/HUD -I./components/adt

TARGET = municipal_system

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

run: $(TARGET)
	./$(TARGET)