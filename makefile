# --------------------------------------------------------------------- #
# Defining directories                                                  #
# --------------------------------------------------------------------- #
CC = clang
COMMON_SRC_DIR = ./src/common
TEST_SRC_DIR = ./src/unit_tests
RTEST_SRC_DIR = ./src/remote_test
RESOURCE_MANAGER_SRC_DIR = ./src/resource_manager
TRAIN_MANAGER_SRC_DIR = ./src/train_manager
PLC_MANAGER_SRC_DIR = ./src/plc

INCLUDE_DIR = ./include

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
CFLAGS += -g

# --------------------------------------------------------------------- #
# Definition of linker options                                          #
# --------------------------------------------------------------------- #
LDFLAGS = -lrt

all: format_code static_analyser build/remote_test/main


format_code:
	clang-format --verbose -i --style=file src/common/*
	clang-format --verbose -i --style=file src/plc/*
	clang-format --verbose -i --style=file src/remote_test/*
	clang-format --verbose -i --style=file src/resource_manager/*
	clang-format --verbose -i --style=file src/train_manager/*
	clang-format --verbose -i --style=file test/*
	clang-format --verbose -i --style=file include/common/*
	clang-format --verbose -i --style=file include/plc/*
	clang-format --verbose -i --style=file include/resource_manager/*
	clang-format --verbose -i --style=file include/train_manager/*

static_analyser:
# 	clang-tidy src/* -- -std=c11 -I include

test:build/test/remote build/test/resource_database\
		build/test/resource_manager_proxy \
		build/test/request_queue \
		build/test/plc_message \
		build/test/plc_facade \
		build/test/plc_proxy \
		build/test/resource_manager \
		build/test/interpreter

	@printf "\n[Unit testing]\n"
	valgrind --tool=memcheck --leak-check=full --show-leak-kinds=all build/test/resource_database -s
	@printf "\n\n"
	valgrind --tool=memcheck --leak-check=full --show-leak-kinds=all build/test/request_queue -s
	@printf "\n\n"
	valgrind --tool=memcheck --leak-check=full --show-leak-kinds=all build/test/resource_manager -s
	@printf "\n\n"
	valgrind --tool=memcheck --leak-check=full --show-leak-kinds=all build/test/resource_manager_proxy -s
	@printf "\n\n"
	valgrind --tool=memcheck --leak-check=full --show-leak-kinds=all build/test/interpreter -s
	@printf "\n\n"
	valgrind --tool=memcheck --leak-check=full --show-leak-kinds=all build/test/plc_message -s
	@printf "\n\n"
	valgrind --tool=memcheck --leak-check=full --show-leak-kinds=all build/test/plc_facade -s
	@printf "\n\n"
	valgrind --tool=memcheck --leak-check=full --show-leak-kinds=all build/test/plc_proxy -s
	@printf "\nDone unit testing\n"


build/test/remote: $(RTEST_SRC_DIR)/main.c \
					$(RTEST_SRC_DIR)/comm.c

	mkdir -p build/test
	$(CC) $(CFLAGS) $^ -o $@

# --------------------------------------------------------------------- #
# Unit testing the resource Manager                                     #
# --------------------------------------------------------------------- #
build/test/request_queue: $(TEST_SRC_DIR)/unit_test_request_queue.c \
								$(RESOURCE_MANAGER_SRC_DIR)/request_queue.c \
								$(COMMON_SRC_DIR)/resource_request.c \
								$(COMMON_SRC_DIR)/verbose.c \
								$(COMMON_SRC_DIR)/flags.c \
								$(COMMON_SRC_DIR)/time_out.c

	mkdir -p build/test
	$(CC) $(CFLAGS) $^ -o $@

build/test/resource_database: $(TEST_SRC_DIR)/unit_test_resource_database.c \
								$(RESOURCE_MANAGER_SRC_DIR)/resource_database.c \
								$(RESOURCE_MANAGER_SRC_DIR)/resource_database_proxy.c \
								$(COMMON_SRC_DIR)/verbose.c \
								$(COMMON_SRC_DIR)/flags.c \
								$(COMMON_SRC_DIR)/time_out.c
	mkdir -p build/test
	$(CC) $(CFLAGS) $^ -o $@

build/test/resource_manager: $(TEST_SRC_DIR)/unit_test_resource_manager.c \
						$(RESOURCE_MANAGER_SRC_DIR)/resource_manager.c \
						$(RESOURCE_MANAGER_SRC_DIR)/resource_database.c \
						$(RESOURCE_MANAGER_SRC_DIR)/request_queue.c \
						$(RESOURCE_MANAGER_SRC_DIR)/resource_database_proxy.c \
						$(COMMON_SRC_DIR)/mock_comm_general.c \
						$(COMMON_SRC_DIR)/mock_resource_request.c \
						$(COMMON_SRC_DIR)/verbose.c \
						$(COMMON_SRC_DIR)/flags.c \
						$(COMMON_SRC_DIR)/time_out.c

	mkdir -p build/test
	$(CC) $(CFLAGS) $^ -o $@

# --------------------------------------------------------------------- #
# Unit testing the Train Manager                                        #
# --------------------------------------------------------------------- #

build/test/resource_manager_proxy: $(TEST_SRC_DIR)/unit_test_resource_manager_proxy.c \
									$(TRAIN_MANAGER_SRC_DIR)/resource_manager_proxy.c \
									$(COMMON_SRC_DIR)/mock_resource_request.c \
									$(COMMON_SRC_DIR)/verbose.c \
									$(COMMON_SRC_DIR)/flags.c
	mkdir -p build/test
	$(CC) -g $(CFLAGS) $^ -o $@

build/test/interpreter: $(TEST_SRC_DIR)/unit_test_interpreter.c \
									$(TRAIN_MANAGER_SRC_DIR)/interpreter.c \
									$(TRAIN_MANAGER_SRC_DIR)/mock_resource_manager_proxy.c \
									$(TRAIN_MANAGER_SRC_DIR)/mock_train.c \
									$(PLC_MANAGER_SRC_DIR)/mock_plc_message.c \
									$(PLC_MANAGER_SRC_DIR)/mock_plc_proxy.c \
									$(PLC_MANAGER_SRC_DIR)/mock_plc_facade.c \
									$(COMMON_SRC_DIR)/mock_resource_request.c \
									$(COMMON_SRC_DIR)/verbose.c \
									$(COMMON_SRC_DIR)/flags.c
	mkdir -p build/test
	$(CC) -g $(CFLAGS) $^ -o $@

# --------------------------------------------------------------------- #
# Unit testing PLC                                                      #
# --------------------------------------------------------------------- #

build/test/plc_message: $(TEST_SRC_DIR)/unit_test_plc_message.c \
									$(PLC_MANAGER_SRC_DIR)/plc_message.c \
									$(COMMON_SRC_DIR)/verbose.c \
									$(COMMON_SRC_DIR)/flags.c
	mkdir -p build/test
	$(CC) -g $(CFLAGS) $^ -o $@

build/test/plc_facade: $(TEST_SRC_DIR)/unit_test_plc_facade.c \
									$(PLC_MANAGER_SRC_DIR)/plc_message.c \
									$(PLC_MANAGER_SRC_DIR)/plc_facade.c \
									$(PLC_MANAGER_SRC_DIR)/model_info.c \
									$(COMMON_SRC_DIR)/verbose.c \
									$(COMMON_SRC_DIR)/flags.c
	mkdir -p build/test
	$(CC) -g $(CFLAGS) $^ -o $@

build/test/plc_proxy: $(TEST_SRC_DIR)/unit_test_plc_proxy.c \
									$(PLC_MANAGER_SRC_DIR)/plc_proxy.c \
									$(PLC_MANAGER_SRC_DIR)/plc_message.c \
									$(PLC_MANAGER_SRC_DIR)/plc_facade.c \
									$(PLC_MANAGER_SRC_DIR)/model_info.c \
									$(COMMON_SRC_DIR)/mock_comm_general.c \
									$(COMMON_SRC_DIR)/time_out.c \
									$(COMMON_SRC_DIR)/verbose.c \
									$(COMMON_SRC_DIR)/flags.c
	mkdir -p build/test
	$(CC) -g $(CFLAGS) $^ -o $@

# --------------------------------------------------------------------- #
# Building Final Version                                                #
# --------------------------------------------------------------------- #

build/resource_manager: $(RESOURCE_MANAGER_SRC_DIR)/resource_manager_main.c \
						$(RESOURCE_MANAGER_SRC_DIR)/resource_manager.c \
						$(RESOURCE_MANAGER_SRC_DIR)/resource_database.c \
						$(RESOURCE_MANAGER_SRC_DIR)/request_queue.c \
						$(RESOURCE_MANAGER_SRC_DIR)/resource_database_proxy.c \
						$(COMMON_SRC_DIR)/comm_general.c \
						$(COMMON_SRC_DIR)/verbose.c \
						$(COMMON_SRC_DIR)/resource_request.c \
						$(COMMON_SRC_DIR)/time_out.c \
						$(COMMON_SRC_DIR)/flags.c
	mkdir -p build
	$(CC) -g $(CFLAGS) $^ -o $@

clean:
	rm -fr build/*
