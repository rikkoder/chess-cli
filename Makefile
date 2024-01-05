ifeq ($(OS),Windows_NT)
	PROGRAM = chess-cli.exe
else
	PROGRAM = chess-cli
endif

# SRC = main.c main_menu.c game.c utilities.c board.c chess_engine.c history.c load_menu.c
SRC_DIR = ./src
BUILD_DIR = ./build
DEBUG_DIR = ./debug
SRC = $(SRC_DIR)/*.c $(SRC_DIR)/ai/*.c
LDFLAGS += -lncurses
CFLAGS += -Wall
CC = gcc
DEBUGFLAGS = -g3 -O0

BASE_DIR = chess-cli-files
SAVE_DIR = .saves
PGN_DIR = pgn-exports

.PHONY: install debug clean uninstall
install: $(BUILD_DIR)/$(PROGRAM)

$(BUILD_DIR)/$(PROGRAM): $(SRC)
	mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $(BUILD_DIR)/$(PROGRAM) $(SRC) $(LDFLAGS)
	mkdir -p $(HOME)/$(BASE_DIR)/$(SAVE_DIR)
	mkdir -p $(HOME)/$(BASE_DIR)/$(PGN_DIR)
	cp $(BUILD_DIR)/$(PROGRAM) $(HOME)/.local/bin

debug: $(DEBUG_DIR)/$(PROGRAM)

$(DEBUG_DIR)/$(PROGRAM): $(SRC)
	mkdir -p $(DEBUG_DIR)
	$(CC) $(CFLAGS) $(DEBUGFLAGS) -o $(DEBUG_DIR)/$(PROGRAM) $(SRC) $(LDFLAGS)

clean:
	rm -rf $(BUILD_DIR)
	rm -rf $(DEBUG_DIR)

uninstall: clean
	rm $(HOME)/.local/bin/$(PROGRAM)
