#include "chess_state.h"
#include <iostream>
#include <iomanip>
#include <cctype>


GameState<ChessMove> * ChessState::GetInitialState() {
  std::string initial_fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
  auto cs = GetStateFromFEN(initial_fen);
  return cs;
}

// Currently ignoring last for fields. Only accounts for current locations
// and current turn.
GameState<ChessMove> * ChessState::GetStateFromFEN(std::string fen) {
  ChessState * cs  = new ChessState();
  char current_field = 1;
  char board_index = 0;
  for(char &c : fen) {
    if(c == ' ') {
      current_field++;
      continue;  
    // Pieces
    } else if(current_field == 1) {
      char unassigned_piece_id = std::tolower(c);
      char piece;
      switch (unassigned_piece_id) {
        case 'p':
          piece = pieces::W_PAWN;
          break;
        case 'n':
          piece = pieces::W_KNIGHT;
          break;
        case 'b':
          piece = pieces::W_BISHOP;
          break;
        case 'r':
          piece = pieces::W_ROOK;
          break;
        case 'q':
          piece = pieces::W_QUEEN;
          break;
        case 'k':
          piece = pieces::W_KING;
          break;
        default:
          piece = pieces::NONE;
          if(c == '/'){
            continue;
          } else if(std::isdigit(c)){
            for(int i=0; i< c-'0'; i++){
              cs->board[board_index] = pieces::NONE;
              board_index++;
            }
            continue;
          }
      }
      
      if(std::islower(c))
        piece |= attrs::BLACK;
     
      cs->board[board_index] = piece;
      board_index++;
    // Current player
    } else if(current_field == 2) {
      cs->current_player = c == 'w' ? player::WHITE : player::BLACK;
    }
  }
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
    std::cout << std::setw(2) << +cs->board[i] << " ";
  }
  std::cout << std::endl;
}
