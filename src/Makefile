CC = gcc
CFLAGS = -Wall -Wextra -g -I./components/HUD -I./components/adt

TARGET = municipal_system

OBJS = main.o components/HUD/hud.o components/adt/struct.o

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

main.o: main.c
	$(CC) $(CFLAGS) -c main.c -o main.o

components/HUD/hud.o: components/HUD/hud.c components/HUD/hud.h
	$(CC) $(CFLAGS) -c components/HUD/hud.c -o components/HUD/hud.o

components/adt/struct.o: components/adt/struct.c components/adt/struct.h
	$(CC) $(CFLAGS) -c components/adt/struct.c -o components/adt/struct.o

clean:
	rm -f $(TARGET) $(OBJS)

run: $(TARGET)
	./$(TARGET)