#include "chess_state.h"
#include <iostream>
#include <iomanip>
#include <cctype>


GameState<ChessMove> * ChessState::GetInitialState() {
  return GetStateFromFEN(fen::starting);
}

void ChessState::createMovesForPiece(char index){
  char * piece = &board[index];
  PieceTracker * pt = trackers[index];
  if(pt == nullptr){
    pt = new PieceTracker(piece, index);
  }
  if(*piece == pieces::PAWN){
    int delta = ownership[index] == player::WHITE ? deltas::UP : deltas::DOWN;
    const int move_pos = index + delta;
    // There's a piece located at delta. Add move to blocked_moves.
    if(board[move_pos]){
      addBlockedMove(pt, move_pos);
    // Square is clear. Add to possible moves.
    } else {
      addPossibleMove(pt, move_pos);
    }
  }
  trackers[index] = pt;
}

void ChessState::createMovesForBoard(){
  for(int i = 0; i < 64; i++){
    createMovesForPiece(i);
  }
}

void ChessState::addMove(MoveMatrix * mm, TwoDIndexVector * indexes_list, PieceTracker * pt, char move_pos) {
  (*mm)[move_pos].push_back(pt);
  int matrix_index = (* mm)[move_pos].size() - 1;
  indexes_list->push_back(std::make_pair(move_pos, matrix_index));
}

void ChessState::addBlockedMove(PieceTracker * pt, char index){
  addMove(&blocked_moves, &(pt->blocked_moves_indexes), pt, index);
}

void ChessState::addPossibleMove(PieceTracker * pt, char index){
  addMove(&possible_moves, &(pt->possible_moves_indexes), pt, index);
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
  
  new_state->ownership[new_location] = new_state->ownership[old_location];

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

void ChessState::PrintState(ChessState * cs, std::string attrs){
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
}
