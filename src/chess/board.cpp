#include "board.hpp"

Board::Board()
{
    int x_pos = (800 / 2) - (4 * 60);
    int y_pos = (600 / 2) - (4 * 60);

    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            tiles[i][j].square.x = x_pos;
            tiles[i][j].square.y = y_pos;
            tiles[i][j].square.w = 60;
            tiles[i][j].square.h = 60;
            x_pos += 60;
        }
        y_pos += 60;
        x_pos = (800 / 2) - (4 * 60);
    }

    resetBoard();
}

void Board::resetBoard()
{
    for (int i = 0; i < 8; i++)
    {
        int side = (i < 2) ? BLACK : WHITE;
        for (int j = 0; j < 8; j++)
        {
            delete tiles[i][j].piece;
            tiles[i][j].piece = nullptr;
            tiles[i][j].colour = NORMAL;

            if (i == 0 || i == 7)
            {
                if (j == 0 || j == 7)
                    tiles[i][j].piece = new Rook(side);
                else if (j == 1 || j == 6)
                    tiles[i][j].piece = new Knight(side);
                else if (j == 2 || j == 5)
                    tiles[i][j].piece = new Bishop(side);
                else if (j == 3)
                    tiles[i][j].piece = new Queen(side);
                else
                    tiles[i][j].piece = new King(side);
            }
            else if (i == 1 || i == 6)
            {
                tiles[i][j].piece = new Pawn(side);
            }
        }
    }
}

void Board::drawBoard(SDL_Renderer* renderer)
{
    SDL_Rect border;
    border.x = (800 / 2) - (4 * 60) - 15;
    border.y = (600 / 2) - (4 * 60) - 15;
    border.w = 60 * 8 + 30;
    border.h = 60 * 8 + 30;

    SDL_SetRenderDrawColor(renderer, 149, 83, 59, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 60, 60, 60, SDL_ALPHA_OPAQUE);
    SDL_RenderFillRect(renderer, &border);

    SDL_Rect inner_board;
    inner_board.x = (800 / 2) - (4 * 60);
    inner_board.y = (600 / 2) - (4 * 60);
    inner_board.w = 60 * 8;
    inner_board.h = 60 * 8;
    SDL_SetRenderDrawColor(renderer, 149, 83, 59, SDL_ALPHA_OPAQUE);
    SDL_RenderFillRect(renderer, &inner_board);

    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            SDL_Color color;
            if (tiles[i][j].colour == CHECK)
                color = {140, 10, 10, SDL_ALPHA_OPAQUE};
            else if (tiles[i][j].colour == SELECT)
                color = {130, 255, 130, SDL_ALPHA_OPAQUE};
            else if (tiles[i][j].colour == DANGER)
                color = {255, 130, 130, SDL_ALPHA_OPAQUE};
            else if ((i + j) % 2 == 0)
                color = {251, 194, 115, SDL_ALPHA_OPAQUE};
            else
                color = {149, 83, 59, SDL_ALPHA_OPAQUE};

            SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
            SDL_RenderFillRect(renderer, &tiles[i][j].square);

            if (tiles[i][j].piece != nullptr && tiles[i][j].piece->image != nullptr)
            {
                if (tiles[i][j].piece->texture == nullptr)
                {
                    tiles[i][j].piece->texture = SDL_CreateTextureFromSurface(renderer, tiles[i][j].piece->image);
                    SDL_SetTextureScaleMode(tiles[i][j].piece->texture, SDL_ScaleModeLinear);
                }
                SDL_RenderCopy(renderer, tiles[i][j].piece->texture, NULL, &tiles[i][j].square);
            }

            if (tiles[i][j].colour == CHECK)
            {
                SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
                SDL_SetRenderDrawColor(renderer, 255, 0, 0, 110);
                SDL_RenderFillRect(renderer, &tiles[i][j].square);
                SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
            }
        }
    }

}

void Board::resetTileColours()
{
    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 8; j++)
            tiles[i][j].colour = NORMAL;
}