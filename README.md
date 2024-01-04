# chess-cli

If you want to play chess without leaving your terminal, you have come to the right place. *chess-cli* is a chess written completely in C using ncurses library for TUI. The program is tested and developed on arch linux but should be compatible with most linux systems and WSL (for windows).

![(screenshot of the game)](https://drive.google.com/uc?export=view&id=1pw5RNw0P2PyW4wOKw39KNQ3ZzHWv5ARx)

## Installation

The project requires *gcc*, *make* and *ncurses* which are pre-installed in most of the linux distributions. If required, you may install these using your system's package manager.
After installing above mentioned dependencies run the following command on terminal:
```bash
git clone https://github.com/rikkoder/chess-cli
make install
```
Now you can simply start the game from terminal by typing `chess-cli`.

## Features
The project is currently under development with some features implemented while other on the way. The project is not fully furnished and may have few bugs, please report if you find any. Following is the list of features completed or to be done:
- [x] 2p local
- [x] ASCII - Unicode mode
- [x] undo
- [x] history (move list)
- [ ] traverse in history
- [x] huds and menu
- [x] save
- [x] load
- [x] pgn export
- [ ] timer
- [ ] vAI
- [ ] difficulty levels
- [ ] 2p LAN
- [ ] ONLINE
- [ ] dynamic size (remove huds for smaller windows)
- [ ] add notation based input
- [ ] add assist mode (show possible moves, threat and check)

