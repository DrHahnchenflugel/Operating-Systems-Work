CC      := gcc
CFLAGS  := -Wall -Wextra -Wpedantic -O2
TARGET  := lab3
SRC     := main.c
OBJ     := $(SRC:.c=.o)

.PHONY: all run clean

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET) $(OBJ)