#include "chess_state.h"
#include <iostream>
#include <iomanip>
GameState<std::string> * ChessState::GetInitialState() {
 ChessBoard initial_board = {{
    {10, 8, 9, 12, 11, 9, 8, 10},
    { 7, 7, 7,  7,  7, 7, 7,  7},
    { 0, 0, 0,  0,  0, 0, 0,  0},
    { 0, 0, 0,  0,  0, 0, 0,  0},
    { 0, 0, 0,  0,  0, 0, 0,  0},
    { 0, 0, 0,  0,  0, 0, 0,  0},
    { 1, 1, 1,  1,  1, 1, 1,  1},
    { 4, 2, 3,  6,  5, 3, 2,  4}
  }};
  ChessState * cs = new ChessState();
  cs->board = initial_board;
  return cs;
}

// Takes a move in the format of piece_location_new_location
// And executes it. Does not check to see if move is legal.
// i.e. e2e4 moves white center right pawn up to spaces.
GameState<std::string> * ChessState::GetNewState(std::string move){
  auto new_state = new ChessState(*this);
  new_state->current_player = !new_state->current_player; 

  auto cm = ChessState::get_indices_for_move(move);
  auto old_location = cm.first;
  auto new_location = cm.second;

  int piece = new_state->board[old_location[0]][old_location[1]];
  new_state->board[old_location[0]][old_location[1]] = pieces::NONE;
  new_state->board[new_location[0]][new_location[1]] = piece;

  return new_state;
}

// Converts a move expressed in the format of a3a4 into proper
// Array indices.
ChessMove ChessState::get_indices_for_move(std::string move){
  auto cm = ChessMove();
  cm.first[0] = 8 - move[1] + '0';
  cm.first[1] = move[0] - 'a';
  cm.second[0] = 8 - move[3] + '0';
  cm.second[1] = move[2] - 'a'; 
  return cm;
}

void ChessState::PrintState(ChessState * cs){
  std::cout << "\nCurrent Player: " << cs->current_player;
  for(int i=0; i<8; i++){
    std::cout << std::endl;
    for(int n=0; n<8; n++){
      std::cout << std::setw(2) << cs->board[i][n] << " ";
    }
  }
  std::cout << std::endl;
}
