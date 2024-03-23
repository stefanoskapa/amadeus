# Compiler and Flags
CC := gcc
CFLAGS := -O3 -g -Wall -Wextra -pedantic -std=c11

# Source and Object Files


LIB := lib/spark.a
SRCS := src/main.c src/uci/uci.c src/eval/eval.c
OBJS := $(SRCS:.c=.o)

SPARK_REPO := https://github.com/stefanoskapa/spark.git
SPARK_DIR := external/spark
SPARK_LIB := $(SPARK_DIR)/build/spark.a
SPARK_INCLUDE_DIR := lib


# Output Executable
TARGET := build/amadeus

# Build Rule
all: spark $(TARGET)

$(TARGET): $(OBJS)
	mkdir -p $(@D)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIB)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@


spark:
	if [ ! -d "$(SPARK_DIR)" ]; then \
     mkdir -p $(SPARK_INCLUDE_DIR); \
     git clone $(SPARK_REPO) $(SPARK_DIR); \
  else \
    cd $(SPARK_DIR) && git pull; \
	fi	
	$(MAKE) -C $(SPARK_DIR)
	cp $(SPARK_LIB) lib/
	cp $(SPARK_DIR)/spark.h $(SPARK_INCLUDE_DIR)/

# Clean Rule
clean:
	rm -f $(OBJS) $(TARGET) lib/spark.a lib/spark.h 
run:
	@build/amadeus
