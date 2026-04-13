CC = gcc
CFLAGS = -Wall -Wextra -std=gnu11
LIBS = -lcurl -lcjson

TARGET = speedtest

SRCS = main.c download.c upload.c json_utils.c location.c server_utils.c
OBJS = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
