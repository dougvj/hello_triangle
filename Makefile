

CFLAGS= -MMD -std=gnu99 
LDFLAGS= -lGL -lGLEW -lSDL2 

EXECUTABLE=hello_triangle

#compilers

CC=gcc



SRC=$(wildcard *.c)
OBJECTS = $(SRC:%.c=%.o)

all: $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $(EXECUTABLE)


clean:
	rm *.o
	rm *.d
	rm $(EXECUTABLE)

-include $(SRC:%.c=%.d) 
-include $(CPPSRC:%.cpp=%.d)

