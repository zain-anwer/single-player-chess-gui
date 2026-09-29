#pragma once
#include <vector>
#include <string>

#include "stockfish_module.hpp"

using namespace std;

#define BLACK 0
#define WHITE 1

enum GameStatus
{
    GAME_IN_PROGRESS,
    GAME_CHECK,
    GAME_CHECKMATE,
    GAME_STALEMATE
};

class ChessInterface {
    
    vector<string> total_moves;
    Stockfish chess_engine;

public:
    
    int turn;

public:
   
    ChessInterface();
    void play_move(string cur_move);
    void reset_game();
    vector<string> list_legal_moves();
    GameStatus game_status();
    string get_eval_score();
    bool check();
    bool stalemate();
    bool checkmate();

    void add_captured_piece(string name, int color);
    vector<string> read_captured_pieces(string color);

    template <typename Game>
    void save_game(Game& board);

    template <typename Game>
    void load_game(Game& board);

    ~ChessInterface();
};
