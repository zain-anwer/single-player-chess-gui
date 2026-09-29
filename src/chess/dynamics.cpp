#include "dynamics.hpp"

Dynamics::Dynamics(ChessInterface *interface)
{
    cout << "Dynamics Initialized\n";
    src_i = -1;
    src_j = -1;
    dest_i = -1;
    dest_j = -1;
    pawn_promotion = false;
    this->interface = interface;
}

// -1 for source selection
// 0 for invalid
// 1 for valid_move
// 2 for valid_capture
// 3 for checkmate
// 4 for stalemate

int Dynamics::select(SDL_Point *p, Board &B)
{
    cout << "Within the select function now\n";
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            if (SDL_PointInRect(p, &B.tiles[i][j].square) == true)
            {
                // source selection

                cout << "src_i: " << this->src_i << ", src_j: " << this->src_j << endl;
                if (this->src_i == -1 && this->src_j == -1)
                {
                    cout << "selecting source\n";
                    
                    if (B.tiles[i][j].piece == nullptr)
                        return INVALID_CHOICE;

                    if (B.tiles[i][j].piece->colour != interface->turn)
                        return INVALID_CHOICE;

                    this->src_i = i;
                    this->src_j = j;

                    cout << "SET TO: src_i = " << src_i << " src_j = " << src_j << " ( i = " << i << " j = " << j << ")" << endl;

                    // colouring tiles upon selection

                    for (string move : interface->list_legal_moves())
                    {
                        if (src_j == (move[0] - 'a') && src_i == ('8' - move[1]))
                        {
                            int d_i = ('8' - move[3]);
                            int d_j = (move[2] - 'a');

                            if (B.tiles[d_i][d_j].piece == nullptr)
                                B.tiles[d_i][d_j].colour = SELECT;

                            else
                                B.tiles[d_i][d_j].colour = DANGER;
                        }
                    }
                    return SOURCE_SELECTION;
                }

                // destination selection

                else
                {
                    cout << "selecting destination\n";

                    // set tile colours to normal
                    // this also helps maintain the check colour
                    // furthermore it helps to signal switching the piece to move 

                    B.resetTileColours();

                    dest_i = i;
                    dest_j = j;

                    if (!validate())
                    {
                        dest_i = -1;
                        dest_j = -1;
                        src_i = -1;
                        src_j = -1;
                        check(B);
                        return INVALID_CHOICE;
                    }

                    // to identify whether its a capture or a simple move
                    
                    bool is_move = false;
                    bool is_capture = false;

                    ChessPiece *captured_piece = B.tiles[dest_i][dest_j].piece;
                    if (captured_piece == nullptr && B.tiles[src_i][src_j].piece->name == "Pawn" && src_j != dest_j)
                        captured_piece = B.tiles[src_i][dest_j].piece;

                    if (captured_piece == nullptr)
                        is_move = true;
                    else
                    {
                        interface->add_captured_piece(captured_piece->name, captured_piece->colour);
                        is_capture = true;
                    }

                    string uci_string = move(B);
                    interface->play_move(uci_string);

                    GameStatus status = interface->game_status();
                    B.resetTileColours();
                    if (status == GAME_CHECK || status == GAME_CHECKMATE)
                    {
                        for (int row = 0; row < 8; row++)
                        {
                            for (int column = 0; column < 8; column++)
                            {
                                ChessPiece *piece = B.tiles[row][column].piece;
                                if (piece != nullptr && piece->colour == interface->turn && piece->name == "King")
                                    B.tiles[row][column].colour = CHECK;
                            }
                        }
                    }

                    if (status == GAME_CHECKMATE)
                        return CHECKMATE;
                    if (status == GAME_STALEMATE)
                        return STALEMATE;

                    if (is_move)
                        return VALID_MOVE;
                    
                    if (is_capture)
                        return VALID_CAPTURE;
                }
            }
        }
    }
    return INVALID_CHOICE;
}

// converts array positions into chess move coordinates to be sent to stockfish engine

string Dynamics::convert()
{
    string cur_move = "xxxx";
    cur_move[1] = ('8' - src_i);
    cur_move[0] = (src_j + 'a');
    cur_move[3] = ('8' - dest_i);
    cur_move[2] = (dest_j + 'a');
    if (pawn_promotion)
    {
        cur_move += 'q';
        pawn_promotion = false;
    }
    return cur_move;
}

bool Dynamics::validate()
{
    string cur_move = "xxxx";
    cur_move[1] = ('8' - src_i);
    cur_move[0] = (src_j + 'a');
    cur_move[3] = ('8' - dest_i);
    cur_move[2] = (dest_j + 'a');
    for (string move : interface->list_legal_moves())
        if (move.size() >= 4 && cur_move == move.substr(0, 4) && (move.size() == 4 || move[4] == 'q'))
            return true;
    return false;
}

/*
a. normal move
b. castling
c. pawn promotion
*/

string Dynamics::move(Board &B)
{
    // handling castling

    string coordinates = convert();

    if (coordinates == "e1g1")
    {
        B.tiles[7][6].piece = B.tiles[7][4].piece;
        B.tiles[7][5].piece = B.tiles[7][7].piece;
        B.tiles[7][4].piece = nullptr;
        B.tiles[7][7].piece = nullptr;
        cout << "The move played is : " << coordinates << endl;
    }

    else if (coordinates == "e1c1")
    {
        B.tiles[7][2].piece = B.tiles[7][4].piece;
        B.tiles[7][3].piece = B.tiles[7][0].piece;
        B.tiles[7][4].piece = nullptr;
        B.tiles[7][0].piece = nullptr;
        cout << "The move played is : " << coordinates << endl;
    }

    else if (coordinates == "e8g8")
    {
        B.tiles[0][6].piece = B.tiles[0][4].piece;
        B.tiles[0][5].piece = B.tiles[0][7].piece;
        B.tiles[0][4].piece = nullptr;
        B.tiles[0][7].piece = nullptr;
        cout << "The move played is : " << coordinates << endl;
    }

    else if (coordinates == "e8c8")
    {
        B.tiles[0][2].piece = B.tiles[0][4].piece;
        B.tiles[0][3].piece = B.tiles[0][0].piece;
        B.tiles[0][4].piece = nullptr;
        B.tiles[0][0].piece = nullptr;
        cout << "The move played is : " << coordinates << endl;
    }

    // handling pawn promotion

    else if (B.tiles[src_i][src_j].piece != nullptr && B.tiles[src_i][src_j].piece->name == "Pawn" && (dest_i == 0 || dest_i == 7))
    {

        int type = B.tiles[src_i][src_j].piece->colour;
        delete B.tiles[src_i][src_j].piece;
        B.tiles[src_i][src_j].piece = nullptr;
        delete B.tiles[dest_i][dest_j].piece;
        B.tiles[dest_i][dest_j].piece = new Queen(type);
        pawn_promotion = true;
        cout << "Pawn promoted to queen by default (didn't have the energy to implement choice :(" << endl;
    }

    else if (B.tiles[src_i][src_j].piece != nullptr && B.tiles[src_i][src_j].piece->name == "Pawn" &&
             B.tiles[dest_i][dest_j].piece == nullptr && src_j != dest_j)
    {
        delete B.tiles[src_i][dest_j].piece;
        B.tiles[src_i][dest_j].piece = nullptr;
        B.tiles[dest_i][dest_j].piece = B.tiles[src_i][src_j].piece;
        B.tiles[src_i][src_j].piece = nullptr;
    }

    // normal moves and captures

    else if (B.tiles[dest_i][dest_j].piece == nullptr && B.tiles[src_i][src_j].piece != nullptr)
    {
        B.tiles[dest_i][dest_j].piece = B.tiles[src_i][src_j].piece;
        B.tiles[src_i][src_j].piece = nullptr;
        cout << "The move played is : " << convert() << endl;
    }

    else if (B.tiles[dest_i][dest_j].piece != nullptr && B.tiles[src_i][src_j].piece != nullptr && B.tiles[dest_i][dest_j].piece->colour != B.tiles[src_i][src_j].piece->colour)
    {
        B.tiles[dest_i][dest_j].piece->endGame = false;
        delete B.tiles[dest_i][dest_j].piece;
        B.tiles[dest_i][dest_j].piece = B.tiles[src_i][src_j].piece;
        B.tiles[src_i][src_j].piece = nullptr;
        cout << "The move played is : " << convert() << endl;
    }

    // restore source & destination indices

    string uci_string = convert();

    src_i = -1;
    src_j = -1;
    dest_i = -1;
    dest_j = -1;

    return uci_string;
}

bool Dynamics::check(Board &B)
{
    if (!interface->check())
        return false;
    cout << "Kings in check right now";
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            if (B.tiles[i][j].piece != nullptr && B.tiles[i][j].piece->colour == interface->turn && B.tiles[i][j].piece->name == "King")
            {
                B.tiles[i][j].colour = CHECK;
                return true;
            }
        }
    }
    return true;
}

void Dynamics::flipBoard(Board &B)
{
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            auto temp_1 = B.tiles[i][j].square;
            B.tiles[i][j].square = B.tiles[7 - i][7 - j].square;
            B.tiles[7 - i][7 - j].square = temp_1;
/*
            auto temp_2 = B.tiles[i][j].picture_square;
            B.tiles[i][j].picture_square = B.tiles[7 - i][7 - j].picture_square;
            B.tiles[7 - i][7 - j].picture_square = temp_2;
*/
        }
    }
}