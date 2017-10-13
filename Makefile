CC = gcc

CFLAGS = -Wall

TARGET = project1

RM = -rm -rf


all: $(TARGET)

$(TARGET): main.c uthread.c uthread.h var.h
	$(CC) $(CFLAGS) -o $(TARGET) main.c uthread.c

clean:
	$(RM) $(TARGET)
