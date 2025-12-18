# 1. Variable Definitions (optional, but good practice)
CC = gcc
C_FILES = nnfit.c nnfit_utils.c
OBJ_FILES = $(C_FILES:.c=.o)
TARGET = nnfit
HEADER = nnfit.h
LDLIBS = libsqlite3.a -lm
CFLAGS = -Wall -g

# 2. Default Target
all: $(TARGET)

# 3. Final Linking Rule
$(TARGET): $(OBJ_FILES)
	$(CC) $(OBJ_FILES) $(LDLIBS) -o $(TARGET) $(CFLAGS)

# 4. Pattern Rule (The key to efficiency!)
# This rule tells 'make' how to compile ANY .c file into a .o file.
%.o: %.c $(HEADER)
	$(CC) -c $< -o $@ $(CFLAGS)

# 5. Clean Target
.PHONY: clean
clean:
	rm -f $(OBJ_FILES) $(TARGET)
