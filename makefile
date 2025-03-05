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
STD = -std=gnu23
CFLAGS += $(STD)
CFLAGS  = -Wall
CFLAGS += -Wextra
CFLAGS += -pedantic
CFLAGS += -Wshadow
CFLAGS += -Wdouble-promotion
CFLAGS += -Wformat=2
CFLAGS += -Wformat-truncation
CFLAGS += -Wundef
CFLAGS += -fno-common
# generate files that analyse stack usage
CFLAGS += -fstack-usage
CFLAGS += -Wconversion
CFLAGS += -I$(INCLUDE_DIR)
CFLAGS += -lm

# --------------------------------------------------------------------- #
# Definition of linker options                                          #
# --------------------------------------------------------------------- #
LDFLAGS = -lrt

all: format_code static_analyser build/remote_test/main

unit_test_ressource_manager:
	clang -I include src/unit_test_ressource_manager.c src/ressource_database.c src/ressource_database_proxy.c -o bin/ressource_manager_unit_test.out

format_code:
	clang-format --verbose -i --style=file src/remote_test/*
	clang-format --verbose -i --style=file src/resource_manager/*
	clang-format --verbose -i --style=file src/train_manager/*
	clang-format --verbose -i --style=file test/*
	clang-format --verbose -i --style=file include/*

static_analyser:
# 	clang-tidy src/* -- $(STD) -I include

test: build/test/comm build/test/unit_test_resource_manager_proxy_cli build/test/unit_test_plc_proxy_cli build/test/unit_test_train
	@printf "\n[Unit testing]\n"
	build/test/comm
# 	build/test/unit_test_resource_manager_proxy_cli
# 	build/test/unit_test_plc_proxy_cli
# 	build/test/unit_test_train
	@printf "\nDone unit testing\n"


build/remote_test/main: src/remote_test/main.c src/remote_test/comm.c
	mkdir -p build/remote_test
	$(CC) $(CFLAGS) $^ -o $@

build/plc_proxy: src/plc_proxy.c
	mkdir -p build
	$(CC) $(CFLAGS) $^ -o $@


build/test/unit_test_resource_manager_proxy_cli: test/unit_test_resource_manager_proxy.c src/resource_manager_proxy_cli.c
	mkdir -p build
	$(CC) -g $(CFLAGS) $^ -o $@

build/test/unit_test_plc_proxy_cli: test/unit_test_plc_proxy.c src/plc_proxy_cli.c src/plc_info_test.c
	mkdir -p build
	$(CC) -g $(CFLAGS) $^ -o $@

build/test/unit_test_train: test/unit_test_train.c src/plc_proxy_cli.c src/plc_info_test.c src/resource_manager_proxy_cli.c src/train.c
	mkdir -p build
	$(CC) -g $(CFLAGS) $^ -o $@

build/test/remote_test/comm: test/remote_test/comm.c src/remote_test/comm.c
	mkdir -p build/test
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm -fr build/*
