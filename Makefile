CC = gcc
CFLAGS = -Wall -Wextra -O2 
LDFLAGS = -lm
TARGET = main_sort
SOURCE = main_sort.c

all: $(TARGET)

$(TARGET): $(SOURCE)
	$(CC) $(CFLAGS) $(SOURCE) -o $(TARGET) $(LDFLAGS)

clean:
	rm -f $(TARGET)

