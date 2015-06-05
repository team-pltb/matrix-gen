TARGET = matrix_gen.out
CC = gcc
CFLAGS=-c -O3 -std=c99 -Wall -Wextra -Wredundant-decls -Wswitch-default \
-Wimport -Wno-int-to-pointer-cast -Wbad-function-cast \
-Wmissing-declarations -Wmissing-prototypes -Wnested-externs \
-Wstrict-prototypes -Wformat-nonliteral -Wundef

.PHONY: default all clean

default: $(TARGET)

OBJECTS = $(patsubst src/%.c, src/%.o, $(wildcard src/*.c))
HEADERS = $(wildcard src/*.h)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

.PRECIOUS: $(TARGET) $(OBJECTS)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) $(LFLAGS) -o $@

clean:
	-rm -f src/*.o
	-rm -f $(TARGET)
