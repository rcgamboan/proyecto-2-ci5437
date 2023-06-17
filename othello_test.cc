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
    
    int test[128] = {12, 13, 14,11,21, 15,9, 26, 5, -1};    
    int npv = 0;
    for( int i = 0; test[i] != -1; ++i ) ++npv;

    // Extract principal variation of the game
    state_t state;
    cout << "Extracting principal variation (PV) with " << npv << " plays ... " << flush;
    for( int i = 0; test[i] != -1; ++i ) {
        bool player = i % 2 == 0; // black moves first!
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

