#include "chess_state.h"
#include <iostream>
#include <iomanip>
GameState<ChessMove> * ChessState::GetInitialState() {
 ChessBoard initial_board = {{
    10, 8, 9, 12, 11, 9, 8, 10,
     7, 7, 7,  7,  7, 7, 7,  7,
     0, 0, 0,  0,  0, 0, 0,  0,
     0, 0, 0,  0,  0, 0, 0,  0,
     0, 0, 0,  0,  0, 0, 0,  0,
     0, 0, 0,  0,  0, 0, 0,  0,
     1, 1, 1,  1,  1, 1, 1,  1,
     4, 2, 3,  6,  5, 3, 2,  4
  }};
  ChessState * cs = new ChessState();
  cs->board = initial_board;
  return cs;
}

// Takes a move in the format of std::pair<int, int> = {old_index, new_index}
// And executes it. Does not check to see if move is legal.
// i.e. e2e4 moves white center right pawn up to spaces.
GameState<ChessMove> * ChessState::GetNewState(ChessMove cm){
  auto new_state = new ChessState(*this);
  new_state->current_player = !new_state->current_player; 

  auto old_location = cm.first;
  auto new_location = cm.second;

  int piece = new_state->board[old_location];
  new_state->board[old_location] = pieces::NONE;
  new_state->board[new_location] = piece;

  return new_state;
}

std::string ChessState::chess_move_to_squares(ChessMove cm){
  std::string move_name = "";
  move_name += index_to_square(cm.first);
  move_name += index_to_square(cm.second);
  return move_name;
}

std::string ChessState::index_to_square(char index){
  std::string square = "";
  square += 'a' + index % 8;
  square += std::to_string(8 - index / 8);
  return square;
}

void ChessState::PrintState(ChessState * cs){
  std::cout << "\nCurrent Player: " << cs->current_player << '\n';
  for(int i=0; i<64; i++){
    if(i % 8 == 0)
      std::cout << std::endl;
    std::cout << std::setw(2) << cs->board[i] << " ";
  }
  std::cout << std::endl;
}
