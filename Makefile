# Compiler and Flags
CC := gcc
CFLAGS := -O3 -g -Wall -Wextra -pedantic -std=c11

# Source and Object Files


LIB := lib/spark.a
SRCS := src/main.c src/uci/uci.c
OBJS := $(SRCS:.c=.o)

# Output Executable
TARGET := build/amadeus

# Build Rule
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIB)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean Rule
clean:
	rm -f $(OBJS) $(TARGET)

run:
	@build/amadeus
