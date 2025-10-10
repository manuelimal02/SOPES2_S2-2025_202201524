CC = gcc
CFLAGS = -Wall -Wextra
LIBS = -lmicrohttpd -lpam -lpam_misc

TARGET = api_server
SRC = main.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC) $(LIBS)

clean:
	rm -f $(TARGET)

run: $(TARGET)
	./$(TARGET)
