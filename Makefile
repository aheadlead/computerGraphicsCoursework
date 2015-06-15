VERSION = debug
OBJ = base_graphics.o draw.o input.o undo.o main.o point.c point_list.c math_extension.o
CC = clang
LIBS = -framework OpenGL \
	   -framework GLUT \
	   -framework Foundation 
TARGET = main

ifeq ($(VERSION), debug)
CFLAGS = -O0 -g -Wall -ftrapv -mllvm -asan-globals
# -ftrapv - integer overflow check
else
CFLAGS = -O3 -DNDEBUG
endif

$(TARGET): $(OBJ)
	$(CC) $(LIBS) $(CFLAGS) $(OBJ) -o $(TARGET)

clean: 
	-rm *.o
	-rm $(TARGET)

run: main
	./main

