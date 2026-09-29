#pragma once
#include <vector>
#include <string>
#include <boost/process/v1.hpp>

namespace bp = boost::process::v1;
using namespace std;

class Stockfish {
  
    bp::ipstream stock_out;
    bp::opstream stock_in;
    bp::child* program;
 
public:
  
    Stockfish();    
    string play_move(vector<string> move_vector);
    vector<string> list_legal_moves();
    void reset_game();
    bool check();
    string get_eval_score();
};