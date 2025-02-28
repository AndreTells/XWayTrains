# --------------------------------------------------------------------- #
# Defining directories                                                  #
# --------------------------------------------------------------------- #
CC = clang
SRC_DIR = ./src/
INCLUDE_DIR = ./include/
BIN_DIR = ./bin/
OBJ_DIR = $(BIN_DIR)/obj

# --------------------------------------------------------------------- #
# Defining compilation options                                          #
#       -Wall           : display all warning messages                  #
#       -Werror         : treat warnings as errors                      #
#       -std=c99        : respect the ISO C99 standard                  #
#       -pedantic       : enforces the C standard as much as possible   #
# --------------------------------------------------------------------- #
CFLAGS  = -Wall
CFLAGS += -std=c99
CFLAGS += -Werror
CFLAGS += -pedantic
CFLAGS += -I$(INCLUDE_DIR)

# --------------------------------------------------------------------- #
# Definition of linker options                                          #
# --------------------------------------------------------------------- #
LDFLAGS = -lrt

format_code:
	clang-format --verbose -i --style=file src/*
	clang-format --verbose -i --style=file include/*

static_analyser:
	clang-tidy src/* -- -std=c11 -I include
