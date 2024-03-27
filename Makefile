CC=gcc
CFLAGS=-Wall -Wextra -pedantic -Wconversion -Wunreachable-code -Wswitch-enum
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

host: host.c
	$(call print_in_color, $(YELLOW), \nCreating bin dir: $(BIN_DIR)\n)
	mkdir -p $(BIN_DIR)
	$(call print_in_color, $(YELLOW), \nCOMPILING $<:\n)
	$(CC) $(CFLAGS) -o $(BIN_DIR)/$@ $<

client: client.c
	$(call print_in_color, $(YELLOW), \nCreating bin dir: $(BIN_DIR)\n)
	mkdir -p $(BIN_DIR)
	$(call print_in_color, $(YELLOW), \nCOMPILING $<:\n)
	$(CC) $(CFLAGS) -o $(BIN_DIR)/$@ $<

clean:
	$(call print_in_color, $(GREEN), \nCleaning...\n)
	rm -rf $(BIN_DIR)
