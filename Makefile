ifeq ($(OS),Windows_NT)
	PROGRAM = chess-cli.exe
else
	PROGRAM = chess-cli
endif

SRC = main.c main_menu.c game.c utilities.c board.c chess_engine.c
LDFLAGS += -lncurses
CFLAGS += -Wall
CC = gcc

.PHONY: all clean
all: $(PROGRAM)

$(PROGRAM): $(SRC)
	$(CC) $(CFLAGS) -o $(PROGRAM) $(SRC) $(LDFLAGS)


clean:
	rm $(PROGRAM)

