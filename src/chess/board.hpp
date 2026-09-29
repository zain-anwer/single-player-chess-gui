#ifndef BOARD_HPP
#define BOARD_HPP

#define NORMAL 0
#define SELECT 1
#define DANGER 2
#define CHECK  3

#define BLACK 0
#define WHITE 1

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