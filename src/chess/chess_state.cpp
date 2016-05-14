#include "chess_state.h"
#include <iostream>
#include <iomanip>
#include <cctype>


GameState<ChessMove> * ChessState::GetInitialState() {
  return GetStateFromFEN(fen::starting);
}

// TODO: Test this code. It's incomplete (I think).
void ChessState::createMovesForPiece(char index){
  char * piece = &board[index];

  PieceTracker * oldpt = trackers[index].front();
  delete oldpt;
  oldpt = nullptr;

  auto pt = new PieceTracker(piece, index);

  if(*piece == pieces::PAWN){
    int delta = ownership[index] == player::WHITE ? deltas::UP : deltas::DOWN;
    const int move_pos = index + delta;
    // There's a piece located at delta. Add move to blocked_moves.
    if(board[move_pos]){
      blocked_moves[move_pos].push_back(pt);
    // Square is clear. Add to possible moves.
    } else {
      possible_moves[move_pos].push_back(pt);
    }
  }
  trackers[index].push_front(pt);
}

void ChessState::createMovesForBoard(){
  for(int i = 0; i < 64; i++){
    createMovesForPiece(i);
  }
}

// Currently ignoring last four fields. Only accounts for current locations
// and current turn.
GameState<ChessMove> * ChessState::GetStateFromFEN(std::string fen) { //{{{
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
          piece = pieces::PAWN;
          break;
        case 'n':
          piece = pieces::KNIGHT;
          break;
        case 'b':
          piece = pieces::BISHOP;
          break;
        case 'r':
          piece = pieces::ROOK;
          break;
        case 'q':
          piece = pieces::QUEEN;
          break;
        case 'k':
          piece = pieces::KING;
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
        cs->ownership[board_index] = player::BLACK;

      cs->board[board_index] = piece;
      board_index++;
    // Current player
    } else if(current_field == 2) {
      cs->current_player = c == 'w' ? player::WHITE : player::BLACK;
    }
  }
  cs->createMovesForBoard();
  return cs;
} // }}}


// Takes a move in the format of std::pair<int, int> = {old_index, new_index}
// And executes it. Does not check to see if move is legal.
// i.e. e2e4 moves white center right pawn up to spaces.
GameState<ChessMove> * ChessState::GetNewState(ChessMove cm){
  auto new_state = new ChessState(*this);
  new_state->current_player = !new_state->current_player;

  auto old_location = cm.first;
  auto new_location = cm.second;

  // Move piece on board
  int piece = new_state->board[old_location];
  new_state->board[old_location] = pieces::NONE;
  new_state->board[new_location] = piece;
  // Update ownership info
  new_state->ownership[new_location] = new_state->ownership[old_location];
  // Delete pointers to tracker in possible_moves and blocked_moves.

  return new_state;
}

/*
void ChessState:removeReferencesToTracker(PieceTracker * pt){
  for(TwoDIndex index : pt->possible_moves_indexes ){
    auto it = possibles_moves[index.first].begin();
    advance(it, index.second);
    possible_moves[index.first].erase(it);
    // Hmm. This is kinda funky. We ruin indexes by doing this. Might want to just nullify it?
  }
}*/

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

char potential_moves_for_rook(char index){ // {{{
  char row = index / 8;
  char col = index % 8;
  unsigned char moves = 0b11111111;

  if(row == 0){
    moves &= ~rook_move::L_UPMOST_RIGHT;
    moves &= ~rook_move::L_UP_RIGHT;
    moves &= ~rook_move::L_UPMOST_LEFT;
    moves &= ~rook_move::L_UP_LEFT;
  } else if(row == 1){
    moves &= ~rook_move::L_UPMOST_RIGHT;
    moves &= ~rook_move::L_UPMOST_LEFT;
  } else if(row == 7){
    moves &= ~rook_move::L_DOWNMOST_RIGHT;
    moves &= ~rook_move::L_DOWN_RIGHT;
    moves &= ~rook_move::L_DOWNMOST_LEFT;
    moves &= ~rook_move::L_DOWN_LEFT;
  } else if(row == 6){
    moves &= ~rook_move::L_DOWNMOST_RIGHT;
    moves &= ~rook_move::L_DOWNMOST_LEFT;
  }

  if(col == 0){
    moves &= ~rook_move::L_UPMOST_LEFT;
    moves &= ~rook_move::L_UP_LEFT;
    moves &= ~rook_move::L_DOWNMOST_LEFT;
    moves &= ~rook_move::L_DOWN_LEFT;
  } else if(col == 1){
    moves &= ~rook_move::L_UP_LEFT;
    moves &= ~rook_move::L_DOWN_LEFT;
  } else if(col == 7){
    moves &= ~rook_move::L_UPMOST_RIGHT;
    moves &= ~rook_move::L_UP_RIGHT;
    moves &= ~rook_move::L_DOWNMOST_RIGHT;
    moves &= ~rook_move::L_DOWN_RIGHT;
  } else if(col == 6){
    moves &= ~rook_move::L_UP_RIGHT;
    moves &= ~rook_move::L_DOWN_RIGHT;
  }
  
  return moves;
} // }}}

void ChessState::PrintState(ChessState * cs, std::string attrs){ // {{{
  std::cout << "\nCurrent Player: " << cs->current_player << "\n";

  for(auto attr : attrs){
    switch(attr){
    // Pieces on board
    case 'p':
      for(int i=0; i<64; i++){
        if(i % 8 == 0)
         std::cout << std::endl;
        std::cout << std::setw(2) << +cs->board[i] << " ";
      }
      break;
    // Ownership of pieces
    case 'o':
      for(int i=0; i<64; i++){
        if(i % 8 ==0)
          std::cout << std::endl;
        std::cout << std::setw(2) << cs->ownership[i] << " ";
      }
      break;
    // Possible move counts
    case 'P':
      for(int i = 0; i< 64; i++){
        if(i % 8 == 0)
          std::cout << std::endl;
        std::cout << std::setw(2) << cs->possible_moves[i].size() << " ";
      }
      break;
    // Blocked move counts
    case 'B':
      for(int i = 0; i< 64; i++){
        if(i % 8 == 0)
          std::cout << std::endl;
        std::cout << std::setw(2) << cs->blocked_moves[i].size() << " ";
      }
      break;
    }

    std::cout << "\n\n";
  }
} // }}}
