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


int main(int argc, const char **argv) {
    state_t pv[128];
    
    int test[128] = {
    12, 21, 26, 13, 22, 18,  7,  6,  5, 27, 33, 23, 17, 11, 19, 15,
    14, 31, 20, 32, 30, 10, 25, 24, 34, 28, 16,  4, 29, 35, 36, -1
};
    int npv = 0;
    for( int i = 0; test[i] != -1; ++i ) ++npv;

    // Extract principal variation of the game
    state_t state;
    cout << "Extracting principal variation (PV) with " << npv << " plays ... " << endl << flush;
    for( int i = 0; test[i] != -1; ++i ) {
        bool player = i % 2 == 0; // black moves first!

        std::vector<int> valid_moves = state.get_valid_moves(player);

        for (int i=0; i<valid_moves.size(); i++){
            cout << valid_moves[i] << " ";
        }
        cout << endl;
        
        int pos = test[i];
        pv[i] = state;
        cout << pv[i];
        state = state.move(player, pos);

        
        
    }
    cout << state;
    pv[0] = state;
    pv[1] = state;
    cout << "done!" << endl;

    



    return 0;
}

