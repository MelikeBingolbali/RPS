# Compiler and flags
CC = gcc
CFLAGS = -Wall -I./include

# Directories
SRCDIR = src
OBJDIR = obj
BINDIR = bin

# Source and object files
SRCS = $(wildcard $(SRCDIR)/*.c)
OBJS = $(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.o,$(SRCS))

# Target executable
TARGET = server

# Libraries
LIBS = -lwebsockets

# Default target
all: $(BINDIR)/$(TARGET)

# Create bin directory if not exists
$(BINDIR):
	mkdir -p $(BINDIR)

# Link the executable
$(BINDIR)/$(TARGET): $(OBJS) | $(BINDIR)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

# Compile source files to object files
$(OBJDIR)/%.o: $(SRCDIR)/%.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Create obj directory if not exists
$(OBJDIR):
	mkdir -p $(OBJDIR)

# Clean up build files
clean:
	rm -f $(OBJDIR)/*.o $(BINDIR)/$(TARGET)

# Phony targets
.PHONY: all clean
