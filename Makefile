ifeq ($(OS),Windows_NT)
	PROGRAM = chess-cli.exe
else
	UNAME_S := $(shell uname -s)
	ifeq ($(UNAME_S),Darwin)
		DMACROS := -D_XOPEN_SOURCE_EXTENDED
	endif
	PROGRAM = chess-cli
endif

# SRC = main.c main_menu.c game.c utilities.c board.c chess_engine.c history.c load_menu.c
SRC_DIR = ./src
BUILD_DIR = ./build
DEBUG_DIR = ./debug
INSTALL_DIR = $(HOME)/.local/bin
SRC = $(SRC_DIR)/*.c $(SRC_DIR)/core/*.c $(SRC_DIR)/ai/*.c $(SRC_DIR)/menus/*.c $(SRC_DIR)/utils/*.c
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
	$(CC) $(CFLAGS) -o $(BUILD_DIR)/$(PROGRAM) $(SRC) $(LDFLAGS) $(DMACROS)
	mkdir -p $(HOME)/$(BASE_DIR)/$(SAVE_DIR)
	mkdir -p $(HOME)/$(BASE_DIR)/$(PGN_DIR)
	@if test ! -d $(INSTALL_DIR); then \
		echo "PLEASE ADD $(INSTALL_DIR) TO YOUR PATH ENVIRONMENT\n";\
		mkdir -p $(INSTALL_DIR);\
	fi
	cp $(BUILD_DIR)/$(PROGRAM) $(INSTALL_DIR)
	@echo -e "\n\nSUCCESSFULLY INSTALLED\n"

debug: $(DEBUG_DIR)/$(PROGRAM)

$(DEBUG_DIR)/$(PROGRAM): $(SRC)
	mkdir -p $(DEBUG_DIR)
	$(CC) $(CFLAGS) $(DEBUGFLAGS) -o $(DEBUG_DIR)/$(PROGRAM) $(SRC) $(LDFLAGS) $(DMACROS)

clean:
	rm -rf $(BUILD_DIR)
	rm -rf $(DEBUG_DIR)

uninstall: clean
	rm $(INSTALL_DIR)/$(PROGRAM)
	@echo -e "\n\nSUCCESSFULLY UNINSTALLED\n"
