OBJ = base_graphics.o draw.o input.o undo.o main.o
CC = gcc
CFLAGS = -framework OpenGL \
		 -framework GLUT \
		 -framework Foundation \
		 -O0

main: $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o main

base_graphics.o: base_graphics.c
	$(CC) -c $(CFLAGS) base_graphics.c -o base_graphics.o

draw.o: draw.c
	$(CC) -c $(CFLAGS) draw.c -o draw.o

input.o: input.c
	$(CC) -c $(CFLAGS) input.c -o input.o

undo.o: undo.c
	$(CC) -c $(CFLAGS) undo.c -o undo.o

