# Компиляторы
CC=g++
CC_FLAGS=-g -W -O1 -pipe -O2 -flto=2 -fno-fat-lto-objects -fuse-linker-plugin -fPIC

# Цели
TARGET=example

# Директории
SRC_DIR=src
BIN_DIR=bin

# Исходные коды
SOURCES=worktime.cpp

DEL_FILE      = rm -f
LIBS_DIRS     = -I./include/
LIBS 		  = $(LIBS_DIRS) -lncursesw

.PHONY: clean build
all: clean build

build: $(SOURCE)
	$(CC) $(CC_FLAGS) $(SOURCES) $(LIBS) -o $(TARGET)

run:
	./$(BIN_DIR)/$(TARGET)

clean:	
	$(DEL_FILE) bin/*
