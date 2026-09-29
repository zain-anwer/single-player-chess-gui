#ifndef BOARD_HPP
#define BOARD_HPP

#define NORMAL 0
#define SELECT 1
#define DANGER 2
#define CHECK  3

#define BLACK 0
#define WHITE 1

constexpr int CHESS_WINDOW_WIDTH = 1040;
constexpr int CHESS_WINDOW_HEIGHT = 800;
constexpr int CHESS_TILE_SIZE = 72;
constexpr int CHESS_BOARD_SIZE = 8 * CHESS_TILE_SIZE;
constexpr int CHESS_BOARD_X = (CHESS_WINDOW_WIDTH - CHESS_BOARD_SIZE) / 2;
constexpr int CHESS_BOARD_Y = (CHESS_WINDOW_HEIGHT - CHESS_BOARD_SIZE) / 2;
constexpr int CHESS_BOARD_BORDER = 15;

#include <SDL2/SDL.h>
#include "tile.hpp"

class Board {

    public:
        Tile tiles[8][8];
    
    public:
		
    	Board();
	    void drawBoard(SDL_Renderer* renderer);
        void resetTileColours();
        void resetBoard();
};

#endif 