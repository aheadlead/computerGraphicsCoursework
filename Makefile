OBJ = base_graphics.o draw.o input.o undo.o main.o point.c point_list.c
CC = gcc
CFLAGS = -O0 -g
LIBS = -framework OpenGL \
	   -framework GLUT \
	   -framework Foundation 
TARGET = main

main: $(OBJ)
	$(CC) $(LIBS) $(CFLAGS) $(OBJ) -o $(TARGET)

clean: 
	-rm *.o

run: main
	./main

base_graphics.o: base_graphics.c
	$(CC) -c $(CFLAGS) base_graphics.c -o base_graphics.o

draw.o: draw.c
	$(CC) -c $(CFLAGS) draw.c -o draw.o

input.o: input.c
	$(CC) -c $(CFLAGS) input.c -o input.o

undo.o: undo.c
	$(CC) -c $(CFLAGS) undo.c -o undo.o

point.o: point.c
	$(CC) -c $(CFLAGS) point.c -o point.o

point_list.o: point_list.c
	$(CC) -c $(CFLAGS) point_list.c -o point_list.o
