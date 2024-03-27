CC=gcc
CFLAGS=-Wall -Wextra -pedantic -Wconversion -Wunreachable-code -Wswitch-enum
CLIBS=-lrt
BIN_DIR=./bin

# COLOR ALIASES
RED=\033[31m
GREEN=\033[32m
YELLOW=\033[33m
BLUE=\033[34m
MAGENTA=\033[35m
CYAN=\033[36m
RESET=\033[0m

# Colored output function
define print_in_color
	@printf "$1"
	@printf "$2"
	@printf "\033[0m"
endef

all: host client

host: bin_dir pool.o host.c
	$(call print_in_color, $(GREEN), \nCOMPILING host.c:\n)
	$(CC) $(CFLAGS) -o $(BIN_DIR)/$@ host.c $(BIN_DIR)/pool.o $(CLIBS)

client: bin_dir pool.o client.c
	$(call print_in_color, $(GREEN), \nCOMPILING client.c:\n)
	$(CC) $(CFLAGS) -o $(BIN_DIR)/$@ client.c $(BIN_DIR)/pool.o $(CLIBS)

bin_dir:
	$(call print_in_color, $(GREEN), \nCreating bin dir: $(BIN_DIR)\n)
	mkdir -p $(BIN_DIR)

pool.o: bin_dir pool.c
	$(call print_in_color, $(BLUE), \nCOMPILING $@\n)
	$(CC) $(CFLAGS) -c pool.c -o $(BIN_DIR)/$@ $(CLIBS)

interface: bin_dir interface.c
	$(call print_in_color, $(BLUE), \nCOMPILING interface.c\n)
	$(CC) $(CFLAGS) interface.c -o $(BIN_DIR)/$@ -lncurses

clean:
	$(call print_in_color, $(GREEN), \nCleaning...\n)
	rm -rf $(BIN_DIR)
