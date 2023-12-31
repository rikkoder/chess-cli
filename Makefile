ifeq ($(OS),Windows_NT)
	PROGRAM = chess-cli.exe
else
	PROGRAM = chess-cli
endif

SRC = main.c main_menu.c game.c utilities.c board.c chess_engine.c history.c load_menu.c
LDFLAGS += -lncurses
CFLAGS += -Wall
CC = gcc
DEBUGFLAGS = -g3 -O0

BASE_DIR = chess-cli-files
SAVE_DIR = .saves
PGN_DIR = pgn-exports

.PHONY: install debug clean uninstall
install: $(PROGRAM)

$(PROGRAM): $(SRC)
	$(CC) $(CFLAGS) -o $(PROGRAM) $(SRC) $(LDFLAGS)
	mkdir -p $(HOME)/$(BASE_DIR)/$(SAVE_DIR)
	mkdir -p $(HOME)/$(BASE_DIR)/$(PGN_DIR)
	cp $(PROGRAM) $(HOME)/.local/bin

debug:
	$(CC) $(CFLAGS) $(DEBUGFLAGS) -o $(PROGRAM) $(SRC) $(LDFLAGS)

clean:
	rm $(PROGRAM)

uninstall: clean
	rm $(HOME)/.local/bin/$(PROGRAM)
