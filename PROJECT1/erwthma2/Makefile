CC=gcc
CFLAGS=-Wall -pthread

# Output executable
OUTPUT=run

# Source files
SRCS=launch.c passenger.c
# Object files
OBJS=$(SRCS:.c=.o)

# Header files
HEADERS=ipc_utils.h

# Default target (the executable)
all: $(OUTPUT)

# How to link the object files to create the final executable
$(OUTPUT): $(OBJS)
	$(CC) $(CFLAGS) -o $(OUTPUT) $(OBJS)

# How to compile .c files to .o files
%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up the compiled files
clean:
	rm -f $(OBJS) $(OUTPUT)

# Debugging targets
.PHONY: clean all
