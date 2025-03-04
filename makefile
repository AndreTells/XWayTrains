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
CFLAGS += -std=gnu23
CFLAGS += -Wextra
CFLAGS += -pedantic
CFLAGS += -I$(INCLUDE_DIR)
CFLAGS += -lm

# --------------------------------------------------------------------- #
# Definition of linker options                                          #
# --------------------------------------------------------------------- #
LDFLAGS = -lrt

all: format_code static_analyser build/main build/test/comm

format_code:
	clang-format --verbose -i --style=file src/* test/*
	# clang-format --verbose -i --style=file include/*

static_analyser:
# 	clang-tidy src/* -- -I include

build/main: src/main.c src/comm.c
	mkdir -p build
	$(CC) $(CFLAGS) $^ -o $@

build/unit_test_resource_manager_proxy_cli: src/unit_test_resource_manager_proxy.c src/resource_manager_proxy_cli.c
	mkdir -p build
	$(CC) -g $(CFLAGS) $^ -o $@

build/unit_test_plc_proxy_cli: src/unit_test_plc_proxy.c src/plc_proxy_cli.c src/plc_info_test.c
	mkdir -p build
	$(CC) -g $(CFLAGS) $^ -o $@

build/unit_test_train: src/unit_test_train.c src/plc_proxy_cli.c src/plc_info_test.c src/resource_manager_proxy_cli.c src/train.c
	mkdir -p build
	$(CC) -g $(CFLAGS) $^ -o $@

build/test/comm: test/comm.c src/comm.c
	mkdir -p build/test
	$(CC) $(CFLAGS) $^ -o $@
	@printf "\n[Unit testing]\n"
	$@
	@printf "\nDone unit testing\n"

clean:
	rm -fr build/*
