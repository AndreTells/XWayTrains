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

unit_test_ressource_manager:
	clang -I include src/unit_test_ressource_manager.c src/ressource_database.c src/ressource_database_proxy.c -o bin/ressource_manager_unit_test.out

format_code:
	clang-format --verbose -i --style=file src/*
	clang-format --verbose -i --style=file include/*

static_analyser:
	clang-tidy src/* -- -std=c11 -I include
