#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/process/v1.hpp>
#include <string>
#include <vector>
#include <iostream>

#include "stockfish_module.hpp"

using namespace std;
namespace bp = boost::process::v1;


Stockfish::Stockfish()
{
    program = new bp::child("stockfish", bp::std_in < stock_in, bp::std_out > stock_out);

    stock_in << "uci" << endl;

    string line;
 
    while (getline(stock_out, line)) {
        if (line == "uciok") break;
    }

    stock_in << "isready" << endl;
 
    while (getline(stock_out, line)) {
        if (line == "readyok") break;
    }

    stock_in << "position startpos" << endl;
    cout << "Stockfish engine initialized" << endl;
}

void Stockfish::set_position(const vector<string> &move_vector)
{
    string line;

    for (const string &str : move_vector)
        line += str + " ";
    stock_in << "position startpos";
    if (!line.empty())
        stock_in << " moves " << line;
    stock_in << endl;
}

vector<string> Stockfish::list_legal_moves(){

    string line; vector<string> legal_moves;
    
    // input to the stockfish game engine

    stock_in << "go perft 1" << endl;

    while (getline(stock_out, line)){
        
        if (!line.compare(0, 5, "Nodes")) break;

        size_t separator = line.find(':');
        if (separator == string::npos)
            continue;

        string move = line.substr(0, separator);
        if (move.size() == 4 || move.size() == 5)
            legal_moves.push_back(move);
    }

    return legal_moves;
}

void Stockfish::reset_game()
{
    stock_in << "position startpos" << endl;
}

// The d command reports checker squares after "Checkers:"; whitespace alone means no check.

bool Stockfish::check()
{
    // flushing the output stream to get rid of any remaining output
    stock_out.clear();

    stock_in << "d" << endl;
    stock_in.flush(); 
    string res;

    while(getline(stock_out,res))
    {
        if (res.empty())
            continue;

        if (!res.compare(0,9,"Checkers:"))
            return res.find_first_not_of(" \t\r", 9) != string::npos;
    }
    return false;
}

string Stockfish::get_eval_score(){
    
    string line, eval_string;
    vector<string> eval_vector;

    stock_in << "eval" << endl;
    
    while (getline(stock_out, line)){
        if (!line.compare(0, 5, "Final")){
            eval_string = line; break;
        }
    }
    
    boost::split(eval_vector, eval_string, boost::is_any_of(" "));

    for (auto i = 0; i < eval_vector.size(); i++)
        if (eval_vector.at(i) == "")
            eval_vector.erase(eval_vector.begin() + i--);

    eval_string = eval_vector.at(2);

    if (eval_string == "none") throw 'C';
    else return eval_string;
}