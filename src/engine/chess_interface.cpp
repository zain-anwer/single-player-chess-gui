#include <string>
#include <vector>
#include <fstream>
#include <iostream>

using namespace std;

#include "chess_interface.hpp"

/* file pointers */
fstream white_pieces_log;
fstream black_pieces_log;

ChessInterface::ChessInterface()
{
    /* opening log files in write mode after truncating prior game content */

    white_pieces_log.open("white_pieces.txt", std::ios::out | std::ios::in | std::ios::trunc);
    black_pieces_log.open("black_pieces.txt", std::ios::out | std::ios::in | std::ios::trunc);
    
    cout << "Interface Initialized\n";   
    turn = WHITE;   
}

ChessInterface::~ChessInterface()
{
    white_pieces_log.close();
    black_pieces_log.close();
}

string ChessInterface::play_move(string cur_move)
{
    cout << "Moved played " << cur_move << " \n"; 
    total_moves.push_back(cur_move);
    
    // toggling the info variable to maintain accurate game state info

    if (turn == WHITE)
        turn = BLACK;
    else
        turn = WHITE;
    
    return chess_engine.play_move(total_moves);
}

void ChessInterface::reset_game()
{
    /* reinitializing file pointers */
    white_pieces_log.close();
    black_pieces_log.close();

    white_pieces_log.open("white_pieces.txt", std::ios::out | std::ios::in | std::ios::trunc);
    black_pieces_log.open("black_pieces.txt", std::ios::out | std::ios::in | std::ios::trunc);
    
    total_moves.clear();
    turn = WHITE;
    chess_engine.reset_game();
}

vector<string> ChessInterface::list_legal_moves()
{    return chess_engine.list_legal_moves();    }

GameStatus ChessInterface::game_status()
{
    vector<string> legal_moves = list_legal_moves();
    bool in_check = chess_engine.check();

    if (legal_moves.empty()) 
    {
        return in_check ? GAME_CHECKMATE : GAME_STALEMATE;
    }
    return in_check ? GAME_CHECK : GAME_IN_PROGRESS;
}

bool ChessInterface::check()
{   return chess_engine.check();    }

bool ChessInterface::checkmate()
{
    return game_status() == GAME_CHECKMATE;
}

bool ChessInterface::stalemate()
{
    return game_status() == GAME_STALEMATE;
}

string ChessInterface::get_eval_score()
{
    
    try { return chess_engine.get_eval_score(); }

    catch (const char error_code){
        switch(error_code){
            case 'C': throw "check";
        }
    }

    return "check";
}

void ChessInterface::add_captured_piece(string name, int color)
{
    if (color == WHITE) 
        white_pieces_log << name << endl;
    else if (color == BLACK) 
        black_pieces_log << name << endl;
    else throw "Invalid Piece Color!";
}

vector<string> ChessInterface::read_captured_pieces(string color){
    
    string piece;
    vector<string> captured_pieces;
    string file_name;

    if (color == "White") 
        file_name = "white_pieces.txt";
    else if (color == "Black") 
        file_name = "black_pieces.txt";
    else throw "Invalid Piece Color!";

    ifstream f_in(file_name);
    while (f_in >> piece)
        captured_pieces.push_back(piece);

    return captured_pieces;
}

template <typename Game>
void ChessInterface::save_game(Game& board){
    ofstream f_out("game_state.bin", ios::out | ios::binary);
    f_out.write((char*) &board, sizeof(board));
    f_out.close();
}

template <typename Game>
void ChessInterface::load_game(Game& board){
    ifstream f_in("game_state.bin", ios::in | ios::binary);
    f_in.read((char*) &board, sizeof(board));
    f_in.close();
}

