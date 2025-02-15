// Game of Othello -- Example of main
// Universidad Simon Bolivar, 2012.
// Author: Blai Bonet
// Last Revision: 1/11/16
// Modified by: Jesus Bandez
//              Roberto Gamboa

#include <iostream>
#include <limits>
#include "othello_cut.h" // won't work correctly until .h is fixed!
#include "utils.h"
#include <climits>

#include <unordered_map>

using namespace std;

unsigned expanded = 0;
unsigned generated = 0;
unsigned long tt_threshold = 32; // threshold to save entries in TT

// Transposition table (it is not necessary to implement TT)
struct stored_info_t {
    int value_;
    int type_;
    enum { EXACT, LOWER, UPPER };
    stored_info_t(int value = -100, int type = LOWER) : value_(value), type_(type) { }
};

struct hash_function_t {
    size_t operator()(const state_t &state) const {
        return state.hash();
    }
};

class hash_table_t : public unordered_map<state_t, stored_info_t, hash_function_t> {
};

hash_table_t TTable[2];

int negamax(state_t state, int depth, int color, bool use_tt = true){

    if (depth == 0 || state.terminal()){
        return color * state.value();
    }

    int alpha = INT_MIN;
    std::vector<int> valid_moves = state.get_valid_moves(color == 1);
    if (valid_moves.size()==0){
        int value = -negamax(state, depth - 1, -color, use_tt);
        if (value > alpha){
            alpha = value;
        }
        ++expanded;
        return alpha;
    }

    for (long unsigned int i = 0; i < valid_moves.size(); i++){
        int pos = valid_moves[i];
        ++generated;
        

        state_t child = state.move(color == 1, pos);        
        if (use_tt){

            int index = color == 1;
    
            auto it = TTable[index].find(child);
                
            if (it != TTable[index].end()){
                // se encuentra el estado en la tabla
                if (it->second.type_ == stored_info_t::EXACT){
                    return it->second.value_;
                }
                else if (it->second.type_ == stored_info_t::LOWER){
                    alpha = max(alpha, it->second.value_);
                }
                else if (it->second.type_ == stored_info_t::UPPER){
                    alpha = min(alpha, it->second.value_);
                }
            }
        }

        int value = -negamax(child, depth - 1, -color, use_tt);
        if (value > alpha){
            alpha = value;
        }

        if (use_tt){
            if (TTable[color==1].size() == tt_threshold){
                TTable[color==1].clear();
            }

            stored_info_t info;
            if (value > alpha){
                info.type_ = stored_info_t::UPPER;
                info.value_ = alpha;                    
            }
            else if (value <= alpha){
                info.type_ = stored_info_t::LOWER;
                info.value_ = value;
            }
            TTable[color==1].insert({child, info});
        }

        
        ++expanded;        
    }
    return alpha;
};

int negamax(state_t state, int depth, int alpha, int beta, int color, bool use_tt = false){

    if (depth == 0 || state.terminal()){
        return color * state.value();
    }

    int score = INT_MIN;
    std::vector<int> valid_moves = state.get_valid_moves(color == 1);
    if (valid_moves.size()==0){
        int value = -negamax(state, depth - 1, -beta, -alpha, -color, use_tt);
        score = max(score, value);
        alpha = max(alpha, value); 
        ++expanded;

        if (use_tt){
            if (TTable[color == 1].size() == tt_threshold){
                TTable[color == 1].clear();
            }
            stored_info_t info;
            if (score <= alpha){
                info.type_ = stored_info_t::UPPER;
            } else if (score >= beta){
                info.type_ = stored_info_t::LOWER;
            } else {
                info.type_ = stored_info_t::EXACT;
            }
            info.value_ = score;
            TTable[color == 1].insert({state, info});
        }

        return score;
    }

    for (long unsigned int i = 0; i < valid_moves.size(); i++){
        int pos = valid_moves[i];
        ++generated;
        state_t child = state.move(color == 1, pos);

        if (use_tt){
            // se busca el estado en la tabla
            int index = color == 1;
    
            auto it = TTable[index].find(child);
                
            if (it != TTable[index].end()){
                if (it->second.type_ == stored_info_t::EXACT){
                    return it->second.value_;
                }
                else if (it->second.type_ == stored_info_t::LOWER){
                    alpha = max(alpha, it->second.value_);
                }
                else if (it->second.type_ == stored_info_t::UPPER){
                    beta = min(beta, it->second.value_);
                }
                if (alpha >= beta){
                    return it->second.value_;
                }

            }             
        } 
        
        int value = -negamax(child, depth - 1, -beta, -alpha, -color, use_tt);
        score = max(score, value);
        alpha = max(alpha, value);
        if (alpha >= beta){
            break;
        }

        if (use_tt){
            if (TTable[color == 1].size() == tt_threshold){
                TTable[color == 1].clear();
            }
            stored_info_t info;
            if (score <= alpha){
                info.type_ = stored_info_t::UPPER;
            } else if (score >= beta){
                info.type_ = stored_info_t::LOWER;
            } else {
                info.type_ = stored_info_t::EXACT;
            }
            info.value_ = score;
            TTable[color == 1].insert({child, info});
        }        
        ++expanded;
        
    }
    return score;

};

bool TEST(state_t state, int depth, int score, int color, string condition){

    if (depth == 0 || state.terminal()){
        if (condition == ">"){
            return state.value() > score;
        } else if (condition == "<"){
            return state.value() < score;
        } else if (condition == "="){
            return state.value() == score;
        } else if (condition == ">="){
            return state.value() >= score;
        } else if (condition == "<="){
            return state.value() <= score;
        }        
    }

    std::vector<int> valid_moves = state.get_valid_moves(color == 1);

    if (valid_moves.size()==0){
        if (color == 1 && TEST(state, depth - 1, score, -color, condition)){
            return true;
        } else if (color == -1 && !TEST(state, depth - 1, score, -color, condition)){
            return false;
        }
        
    }
    
    for (long unsigned int i = 0; i < valid_moves.size(); i++){
        int pos = valid_moves[i];
        state_t child = state.move(color == 1, pos);
        
        if (color == 1 && TEST(child, depth - 1, score, -color, condition)){
            return true;
        } else if (color == -1 && !TEST(child, depth - 1, score, -color, condition)){
            return false;
        }
        
        
    }
    return !(color == 1);

}

int scout(state_t state, int depth, int color, bool use_tt = false){

    if (depth == 0 || state.terminal()){
        
        return state.value();
    }

    std::vector<int> valid_moves = state.get_valid_moves(color == 1);
    int score = 0;

    if (valid_moves.size()==0){
        score = scout(state, depth - 1, -color, use_tt);
        ++expanded;
    }

    for (long unsigned int i = 0; i < valid_moves.size(); i++){
        int pos = valid_moves[i];
        ++generated;
        state_t child = state.move(color == 1, pos);
        
        if (use_tt){
            int index = color == 1;
    
            auto it = TTable[index].find(child);
                
            if (it != TTable[index].end()){
                // se encuentra el estado en la tabla
                if (it->second.type_ == stored_info_t::EXACT){
                    return it->second.value_;
                }
                else if (it->second.type_ == stored_info_t::LOWER){
                    score = max(score, it->second.value_);
                }
                else if (it->second.type_ == stored_info_t::UPPER){
                    score = max(score, it->second.value_);
                }
            }
        }
        
        if (i == 0){            
            score = scout(child, depth - 1, -color, use_tt);
        } else {
            // Max node
            if (color == 1 && TEST(child, depth, score, -color, ">")){
                score = scout(child, depth - 1, -color, use_tt);
            } 
            if (color == -1 && !TEST(child, depth, score, -color, ">=")){
                score = scout(child, depth - 1, -color, use_tt);
            }
        }
        
        if (use_tt){
            if (TTable[color==1].size() == tt_threshold){
                TTable[color==1].clear();
            }
            stored_info_t info = stored_info_t(score, stored_info_t::EXACT);
            TTable[color==1].insert({child, info});  
        }
        ++expanded;
    }
    return score;
};

int negascout(state_t state, int depth, int alpha, int beta, int color, bool use_tt = false) {

    if (use_tt) {
        auto iter = TTable[color==1].find(state);
        if (iter != TTable[color==1].end()) {
            const stored_info_t& stored_info = iter->second;
            if (stored_info.type_ == stored_info_t::EXACT) {
                return stored_info.value_;
            } else if (stored_info.type_ == stored_info_t::LOWER) {
                alpha = max(alpha, stored_info.value_);
            } else if (stored_info.type_ == stored_info_t::UPPER) {
                beta = min(beta, stored_info.value_);
            }
            if (alpha >= beta) {
                return stored_info.value_;
            }
        }
    }

    if (depth == 0 || state.terminal()) {
        return color * state.value();
    }

    int original_alpha = alpha;

    std::vector<int> valid_moves = state.get_valid_moves(color == 1);
    if (valid_moves.size() == 0) {
        int value = -negascout(state, depth - 1, -beta, -alpha, -color, use_tt);
        alpha = max(alpha, value);
        ++expanded;
    }

    for (size_t i = 0; i < valid_moves.size(); i++) {
        int pos = valid_moves[i];
        ++generated;
        state_t child = state.move(color == 1, pos);
        int score;

        if (i == 0) {
            score = -negascout(child, depth - 1, -beta, -alpha, -color, use_tt);
        } else {
            score = -negascout(child, depth - 1, -alpha - 1, -alpha, -color, use_tt);
            if (alpha < score && score < beta) {
                score = -negascout(child, depth - 1, -beta, -score, -color, use_tt);
            }
        }

        alpha = max(alpha, score);

        if (alpha >= beta) {
            break;
        }

        ++expanded;
    }

    if (use_tt){
        stored_info_t stored_info;
        if (alpha <= original_alpha) {
            stored_info.value_ = alpha;
            stored_info.type_ = stored_info_t::UPPER;
        } else if (alpha >= beta) {
            stored_info.value_ = alpha;
            stored_info.type_ = stored_info_t::LOWER;
        } else {
            stored_info.value_ = alpha;
            stored_info.type_ = stored_info_t::EXACT;
        }
        TTable[color==1][state] = stored_info;
    }

    return alpha;
};


int main(int argc, const char **argv) {
    state_t pv[128];
    int npv = 0;
    for( int i = 0; PV[i] != -1; ++i ) ++npv;

    int algorithm = 0;
    if( argc > 1 ) algorithm = atoi(argv[1]);
    bool use_tt = argc > 2;

    // Extract principal variation of the game
    state_t state;
    cout << "Extracting principal variation (PV) with " << npv << " plays ... " << flush;
    for( int i = 0; PV[i] != -1; ++i ) {
        bool player = i % 2 == 0; // black moves first!
        int pos = PV[i];
        pv[npv - i] = state;
        state = state.move(player, pos);
    }
    pv[0] = state;
    cout << "done!" << endl;


    // Print name of algorithm
    cout << "Algorithm: ";
    if( algorithm == 1 )
        cout << "Negamax (minmax version)";
    else if( algorithm == 2 )
        cout << "Negamax (alpha-beta version)";
    else if( algorithm == 3 )
        cout << "Scout";
    else if( algorithm == 4 )
        cout << "Negascout";
    cout << (use_tt ? " w/ transposition table" : "") << endl;

    // Run algorithm along PV (bacwards)
    cout << "Moving along PV:" << endl;
    for( int i = 0; i <= npv; ++i ) {
        cout << pv[i];
        int value = 0;
        TTable[0].clear();
        TTable[1].clear();
        float start_time = Utils::read_time_in_seconds();
        expanded = 0;
        generated = 0;
        int color = i % 2 == 1 ? 1 : -1;

        try {
            if( algorithm == 1 ) {
                value = negamax(pv[i], 35, color, use_tt);
            } else if( algorithm == 2 ) {
                value = negamax(pv[i], 35, -200, 200, color, use_tt);
            } else if( algorithm == 3 ) {                
                value = scout(pv[i], 35, color, use_tt);
            } else if( algorithm == 4 ) {
                value = negascout(pv[i], 35, -200, 200, color, use_tt);
            }
        } catch( const bad_alloc &e ) {
            cout << "size TT[0]: size=" << TTable[0].size() << ", #buckets=" << TTable[0].bucket_count() << endl;
            cout << "size TT[1]: size=" << TTable[1].size() << ", #buckets=" << TTable[1].bucket_count() << endl;
            use_tt = false;
        }

        float elapsed_time = Utils::read_time_in_seconds() - start_time;

        cout << npv + 1 - i << ". " << (color == 1 ? "Black" : "White") << " moves: "
             << "value=" << color * value
             << ", #expanded=" << expanded
             << ", #generated=" << generated
             << ", seconds=" << elapsed_time
             << ", #generated/second=" << generated/elapsed_time
             << endl;
    }

    return 0;
}

