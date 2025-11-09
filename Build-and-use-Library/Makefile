# Compiler and flags
CC = gcc
CFLAGS = -Wall -fPIC

# Targets and library names
TARGET_STATIC = main_static
TARGET_SHARED = main_shared
LIB_STATIC = libstrutils.a
LIB_SHARED = libstrutils.so

# Default target: build both versions
all: $(TARGET_STATIC) $(TARGET_SHARED)

# Build static library (.a)
$(LIB_STATIC): strutils.o
	ar rcs $(LIB_STATIC) strutils.o

# Build shared library (.so)
$(LIB_SHARED): strutils.o
	$(CC) -shared -o $(LIB_SHARED) strutils.o

# Compile object file
strutils.o: strutils.c strutils.h
	$(CC) $(CFLAGS) -c strutils.c

# Build executable linked with static library
$(TARGET_STATIC): main.c $(LIB_STATIC)
	$(CC) main.c -L. -lstrutils -o $(TARGET_STATIC)

# Build executable linked with shared library
$(TARGET_SHARED): main.c $(LIB_SHARED)
	$(CC) main.c -L. -lstrutils -o $(TARGET_SHARED)

# Clean up build files
clean:
	rm -f *.o *.a *.so $(TARGET_STATIC) $(TARGET_SHARED)

.PHONY: all clean

