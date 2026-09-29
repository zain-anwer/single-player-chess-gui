# Single-Player Chess GUI

A C++ single-player chess application with an SDL2 graphical interface and Stockfish engine integration through Boost.Process and the UCI protocol.

## Table of Contents

* [Overview](#overview)
* [Architecture](#architecture)
* [Features](#features)
* [Project Structure](#project-structure)
* [Stockfish Communication](#stockfish-communication)
* [Game Flow](#game-flow)
* [Logging and Game State](#logging-and-game-state)
* [Dependencies](#dependencies)
* [Building](#building)
* [Running](#running)
* [Controls](#controls)

---

## Overview

The application provides a graphical chess board where the player interacts with the game through mouse input. Move legality and engine responses are handled through Stockfish, which runs as a separate process and communicates with the application using pipes.

The application is organized into four main areas:

* **GUI** — SDL2 window management, input handling, and rendering
* **Chess Logic** — Board, tiles, pieces, and move handling
* **Engine Layer** — Game-state coordination and Stockfish communication
* **Utilities** — Audio playback and supporting functionality

---

## Architecture

### High-Level Architecture

```text
                         ┌──────────────────────┐
                         │       main.cpp       │
                         │      SDL2 GUI        │
                         │                      │
                         │ • Input / Events     │
                         │ • Rendering          │
                         │ • Game UI            │
                         │ • Audio coordination │
                         └──────────┬───────────┘
                                    │
                                    ▼
                    ┌──────────────────────────────┐
                    │       Chess Logic            │
                    │                              │
                    │  Board ──► Tile ──► Piece    │
                    │                    ├─ Pawn   │
                    │                    ├─ Knight │
                    │                    ├─ Bishop │
                    │                    ├─ Rook   │
                    │                    ├─ Queen  │
                    │                    └─ King   │
                    │                              │
                    │  Dynamics                    │
                    │  • Selection                 │
                    │  • Move validation           │
                    │  • Movement / special moves  │
                    │  • Board orientation         │
                    └──────────────┬───────────────┘
                                   │
                                   ▼
                    ┌──────────────────────────────┐
                    │      ChessInterface          │
                    │                              │
                    │  • Turn tracking             │
                    │  • Move history              │
                    │  • Game status               │
                    │  • Captured-piece logging    │
                    │  • Engine coordination       │
                    └──────────────┬───────────────┘
                                   │
                                   ▼
                    ┌───────────────────────────────┐
                    │    Stockfish Module           │
                    │                               │
                    │  • Boost.Process              │
                    │  • UCI communication          │
                    │  • Command / response parsing │
                    └──────────────┬────────────────┘
                                   │
                            stdin / stdout
                                   │
                                   ▼
                         ┌──────────────────┐
                         │    Stockfish     │
                         │  External Engine │
                         └──────────────────┘

                    ┌──────────────────────────────┐
                    │          Utilities           │
                    │                              │
                    │  Audio                       │
                    │  • Move sounds               │
                    │  • Capture sounds            │
                    │  • Illegal-move sounds       │
                    └──────────────────────────────┘
```

### Typical Move Flow

```text
Player
  │
  │ clicks piece
  ▼
SDL2 Event Loop
  │
  ▼
Dynamics::select()
  │
  │ selects source and destination
  ▼
Dynamics::validate()
  │
  │ checks Stockfish legal moves
  ▼
Dynamics::move()
  │
  ├── Normal move
  ├── Capture
  ├── Castling
  ├── En passant
  └── Promotion
  │
  ▼
Dynamics::convert()
  │
  │ e.g. e2e4
  ▼
ChessInterface::play_move()
  │
  ├── Updates move history
  ├── Updates turn
  └── Sends position to Stockfish
          │
          ▼
      Stockfish
          │
          │ bestmove
          ▼
      ChessInterface
          │
          ▼
      Board / GUI update
          │
          ├── Captured-piece logging
          └── Audio feedback
```

---

## Features

### Graphical Interface

* SDL2-based graphical chess board
* 8×8 board representation
* BMP textures for chess pieces
* Highlighting for:

  * Selected squares
  * Legal destinations
  * Captures
  * Check
* Captured-piece trays
* Game-over interface
* Board orientation can be flipped between turns

### Move Handling

The game uses a two-click interaction model:

1. Select a piece.
2. Select its destination.

Moves are validated against the legal moves provided by Stockfish.

The application handles:

* Normal moves
* Captures
* Castling
* En passant
* Pawn promotion to queen
* Check
* Checkmate
* Stalemate

### Stockfish Integration

Stockfish runs as an external process and communicates with the application through Boost.Process.

The application uses the UCI protocol for:

* Engine initialization
* Readiness synchronization
* Setting the board position
* Requesting engine moves
* Obtaining legal moves
* Check detection
* Position evaluation

### Audio Feedback

Different sounds are played depending on the move:

* `illegal.wav` — illegal move
* `move.wav` — valid move
* `capture.wav` — capture

---

## Project Structure

```text
.
├── src/
│   ├── main.cpp
│   │
│   ├── chess/
│   │   ├── board.hpp
│   │   ├── board.cpp
│   │   ├── piece.hpp
│   │   ├── piece.cpp
│   │   ├── tile.hpp
│   │   ├── tile.cpp
│   │   ├── dynamics.hpp
│   │   └── dynamics.cpp
│   │
│   ├── engine/
│   │   ├── chess_interface.hpp
│   │   ├── chess_interface.cpp
│   │   ├── stockfish_module.hpp
│   │   └── stockfish_module.cpp
│   │
│   └── utils/
│       ├── audio.hpp
│       └── audio.cpp
│
├── ChessPieceImages/
│   ├── King-B.bmp
│   ├── King-W.bmp
│   ├── Queen-B.bmp
│   ├── Queen-W.bmp
│   ├── Rook-B.bmp
│   ├── Rook-W.bmp
│   ├── Bishop-B.bmp
│   ├── Bishop-W.bmp
│   ├── Knight-B.bmp
│   ├── Knight-W.bmp
│   ├── Pawn-B.bmp
│   └── Pawn-W.bmp
│
├── ChessAudio/
│   ├── illegal.wav
│   ├── move.wav
│   └── capture.wav
│
├── Makefile
├── .gitignore
└── README.md
```

### Core Classes

#### `Board`

Maintains the 8×8 chess board and its tiles.

Responsibilities include:

* Initial board setup
* Piece placement
* Board rendering
* Tile colour management
* Board reset

#### `Tile`

Represents an individual board square.

A tile maintains:

* Its screen position
* Its visual state
* The piece occupying it

#### `ChessPiece`

Base class for chess pieces.

Derived classes:

```text
ChessPiece
├── Pawn
├── Knight
├── Bishop
├── Rook
├── Queen
└── King
```

Each piece maintains properties such as its colour, name, value, image, and SDL texture.

#### `Dynamics`

Handles interaction between the GUI and chess board.

Responsibilities include:

* Piece selection
* Destination selection
* Move validation
* Move execution
* Castling
* En passant
* Promotion
* Board flipping
* UCI move conversion

#### `ChessInterface`

Acts as the coordinator between the chess GUI and the Stockfish engine.

It maintains:

* Current turn
* Move history
* Game status
* Captured-piece logs
* Stockfish interaction

#### `Stockfish`

Provides the interface to the external Stockfish process.

It handles:

* Process creation
* IPC
* UCI commands
* Engine responses
* Legal-move generation
* Engine moves
* Check detection
* Position evaluation

---

## Stockfish Communication

The Stockfish module uses **Boost.Process** to launch Stockfish as a separate process.

Communication occurs through two pipes:

```text
┌───────────────────────┐
│   Chess Application   │
│                       │
│   opstream ────────────────► stdin
│                       │
│   ipstream ◄──────────────── stdout
└───────────────────────┘
             │
             ▼
      ┌─────────────┐
      │  Stockfish  │
      └─────────────┘
```

### UCI Initialization

When the Stockfish module is initialized:

```text
Application ──► uci
Stockfish  ───► uciok

Application ──► isready
Stockfish  ───► readyok

Application ──► position startpos
```

### Setting a Position

The complete move history is sent to Stockfish:

```text
position startpos moves e2e4 e7e5 g1f3
```

This allows Stockfish to reconstruct the current position.

### Requesting an Engine Move

The application requests a move using:

```text
go movetime 300
```

Stockfish responds with information lines followed by:

```text
bestmove e4e5
```

The application extracts the move following `bestmove`.

### Obtaining Legal Moves

Legal moves are obtained using:

```text
go perft 1
```

Stockfish returns entries such as:

```text
e2e4: 1
g1f3: 1
b1c3: 1
```

The application parses the moves from these responses.

### Check Detection

The `d` command is used to request a board description:

```text
d
```

The response contains a `Checkers:` field. The application uses this field to determine whether the current position is in check.

### Position Evaluation

The application requests an evaluation using:

```text
eval
```

The resulting `Final evaluation:` value is parsed and returned by the engine interface.

---

## Game Flow

A typical game proceeds as follows:

```text
Application Start
       │
       ▼
Initialize SDL2
       │
       ▼
Initialize Board
       │
       ▼
Initialize ChessInterface
       │
       ▼
Start Stockfish
       │
       ├── uci
       ├── isready
       └── position startpos
       │
       ▼
     Game Loop
       │
       ▼
Player selects piece
       │
       ▼
Legal moves requested
       │
       ▼
Player selects destination
       │
       ▼
Move validated
       │
       ▼
Board updated
       │
       ▼
Move sent to Stockfish
       │
       ▼
Stockfish returns best move
       │
       ▼
Game status checked
       │
       ├── Game continues
       │
       └── Checkmate / Stalemate
```

---

## Logging and Game State

### Captured Pieces

Captured pieces are stored in two text files:

```text
white_pieces.txt
black_pieces.txt
```

The files are reset when a new game is initialized.

They contain the names of captured pieces.

For example:

```text
Pawn
Pawn
Knight
Queen
```

The `ChessInterface` provides:

```cpp
add_captured_piece()
read_captured_pieces()
```

for writing and reading these logs.

### Console Logging

The application also produces diagnostic information through standard output, including events such as:

* Piece selection
* Destination selection
* Move conversion
* UCI moves
* Board orientation changes
* Engine interaction

---

## Game State

The interface maintains the current turn using the `WHITE` and `BLACK` states.

Move history is stored as a vector of UCI move strings:

```text
e2e4
e7e5
g1f3
...
```

This history is used when constructing the Stockfish `position` command.

The game status can be:

```text
GAME_IN_PROGRESS
GAME_CHECK
GAME_CHECKMATE
GAME_STALEMATE
```

---

## Dependencies

### Required Software

| Dependency       | Purpose                               |
| ---------------- | ------------------------------------- |
| C++17 compiler   | Compiling the application             |
| Make             | Build automation                      |
| SDL2             | Windowing, rendering, input and audio |
| Boost.Process    | Stockfish process management and IPC  |
| Boost.Filesystem | Filesystem functionality              |
| Boost.Algorithm  | String parsing utilities              |
| Stockfish        | Chess engine                          |

### Ubuntu / Debian

```bash
sudo apt update
sudo apt install g++ make libsdl2-dev libboost-system-dev libboost-filesystem-dev stockfish
```

Ensure Stockfish is available:

```bash
which stockfish
```

---

## Building

### 1. Clone the Repository

```bash
git clone https://github.com/zain-anwer/single-player-chess-gui.git
cd single-player-chess-gui
```

### 2. Build

Run:

```bash
make
```

For a clean rebuild:

```bash
make clean
make
```

The resulting executable is:

```text
chess_app
```

### 3. Clean Build Files

```bash
make clean
```

This removes generated object files and the executable.

---

## Running

From the repository root:

```bash
./chess_app
```

The application requires the Stockfish executable and the asset directories to be available from the expected working directory.

The required assets are:

```text
ChessPieceImages/
ChessAudio/
```

---

## Controls

| Input               | Action                        |
| ------------------- | ----------------------------- |
| Left mouse click    | Select a piece or destination |
| Window close button | Exit application              |
| Restart button      | Restart after game over       |
| Quit button         | Exit after game over          |
| ESC / ENTER         | Handle game-over interaction  |

---

## Notes

* Stockfish must be accessible through the system `PATH`.
* The application expects the chess-piece images and audio assets to be available in their respective directories.
* Captured-piece log files are generated at runtime.
* Object files and generated build/runtime files can be excluded using `.gitignore`.

---

## License

Apache License 2.0 — see the `LICENSE` file for details.
