BIN = hdbg
CC = gcc
CFLAGS = -Wall -Werror -Wextra -pedantic -std=c99
DEBUG = 1
SRC = $(wildcard src/*.c)

ifeq ($(DEBUG),1)
	CFLAGS += -g -O0
endif

.PHONY: help
help:
	@echo
	@echo "targets:"
	@echo
	@echo "  build    build the project"
	@echo "  lint     run the clang-format linter"
	@echo

.PHONY: build
build:
	$(CC) $(CFLAGS) -o build/$(BIN) $(SRC)

OBJ = $(shell find src -type f -iname '*.h' -or -iname '*.c')

.PHONY: lint
lint: $(OBJ)
	@clang-format -style=file -i $(OBJ)
	@echo "reformatted successfully"
