#include "chess_state.h"
#include <iostream>
#include <iomanip>
#include <cctype>
#include <map>
#include <assert.h>
#include <bitset>

ChessState::~ChessState() { // {{{
  for(auto tracker_collection : trackers){
    for(auto tracker : tracker_collection){
      tracker.reset();
    }
  }
} // }}}

// Return a pointer to a deepcopy of the ChessState.
// Specifically deep copies trackers, possible_moves, and blocked_moves.
ChessState * ChessState::DeepCopy(){ // {{{
  // Shallow copy
  auto copy = new ChessState(*this);

  /*
    Keys set equal to old pointers to piece_trackers
    values set equal to copy of those piece_trackers
  */
  std::map<SharedPieceTracker, SharedPieceTracker *> pointer_map;
  // Copy trackers and populate pointer_map
  for(int i = 0; i < trackers.size(); ++i){
    // Clear stuff...
    copy->trackers[i].clear();
    for(auto & tracker : trackers[i]){
      // Should we be deep copying trackers? Don't think so.
      auto pt = std::make_shared<PieceTracker>(tracker->piece, tracker->index);
      copy->trackers[i].push_front(pt);
      // Ehh this might be wrong...
      pointer_map[tracker] = &copy->trackers[i].front();
    }
  }
  for(auto & moveset : copy->possible_moves){
    for(auto & move : moveset){
      move = pointer_map[*move];
    }
  }
  for(auto & moveset : copy->blocked_moves){
    for(auto & move : moveset){
      move = pointer_map[*move];
    }
  }
  return copy;
} // }}}

GameState<ChessMove> * ChessState::GetInitialState() { //{{{
  return GetStateFromFEN(fen::starting);
} // }}}

// TODO: Test this code. It's incomplete (I think).
void ChessState::CreateMovesForPiece(char index){ // {{{
  std::cout << "CreateMovesForPiece(" << +index << ")\n";
  char * piece = &board[index];
  if(trackers[index].size() > 0){
    trackers[index].front() = nullptr;
  }

  trackers[index].push_front(std::make_shared<PieceTracker>(piece, index));
  auto pt = &trackers[index].front();

  std::vector<char> deltas;
  if(*piece == pieces::PAWN){
    const int delta = ownership[index] == player::WHITE ? deltas::UP : deltas::DOWN;
    deltas.push_back(delta);
  } else {
    if ((*piece & attrs::DIAGONAL) == attrs::DIAGONAL){
      // Stub
    }
    if ((*piece & attrs::ADJACENT) == attrs::ADJACENT){
      AddDeltaRange(deltas, index, deltas::RIGHT, 0, -1);
    }
  }
  AddPieceTrackerToDeltas(pt, deltas);
} // }}}

// TODO: test.
void ChessState::AddDeltaRange(std::vector<char> & deltas, int index, int delta, int rowBound, int colBound){ // {{{
  char piece = board[index];
  bool isSliding = (piece & attrs::SLIDING) == attrs::SLIDING;
  std::cout << "Sliding? " << isSliding << '\n';

  for(int deltaTotal = delta;
      (index + deltaTotal) % 8 != rowBound                   // Ensure we stay within row boundary.
      && (index + deltaTotal) / 8 != colBound                // Ensure we stay within col boundary.
      && (isSliding || index + deltaTotal == index + delta); // Continue running if piece slides. If not run once.
      deltaTotal += delta
  ){
    deltas.push_back(deltaTotal);
    // Stop adding to deltas if we run into a piece on the board.
    if(board[index + deltaTotal] != pieces::NONE){
      std::cout << "BREAK!!! " << +board[index + deltaTotal] << '\n';
      break;
    }
  }
} // }}}

// Takes a PieceTracker and a list of move deltas. Adds
// PieceTracker to either blocked or possible moves for each
// delta depending on the existence of a piece at delta position.
void ChessState::AddPieceTrackerToDeltas(SharedPieceTracker * pt, const std::vector<char> & deltas){ // {{{
  const char index = (*pt)->index;
  for(int delta : deltas){
    const int move_pos = index + delta;
    // There's a piece located at delta. Add move to blocked_moves.
    if(board[move_pos]){
      blocked_moves[move_pos].push_back(pt);
    // Square is clear. Add to possible moves.
    } else {
      possible_moves[move_pos].push_back(pt);
    }
  }
} // }}}

// Calculate moves for every piece on board
void ChessState::CreateMovesForBoard(){ // {{{
  for(int i = 0; i < NUM_SQUARES; i++){
    CreateMovesForPiece(i);
  }
} // }}}

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
  cs->CreateMovesForBoard();
  return cs;
} // }}}


// Makes a DeepCopy of this, executes ModifyState on it and returns pointer to it.
GameState<ChessMove> * ChessState::GetNewState(ChessMove cm){ // {{{
  auto new_state = this->DeepCopy();
  return new_state->ModifyState(cm);
} // }}}

// Takes a move in the format of std::pair<char, char> = {old_index, new_index}
// And executes it. Does not check to see if move is legal.
// i.e. e2e4 moves white center right pawn up to spaces.
GameState<ChessMove> * ChessState::ModifyState(ChessMove cm){ // {{{
  current_player = !current_player;

  auto old_location = cm.first;
  auto new_location = cm.second;

  // Move piece on board
  int piece = board[old_location];
  board[old_location] = pieces::NONE;
  board[new_location] = piece;
  // Update ownership info
  ownership[new_location] = ownership[old_location];
  // Recalculate blocked/possible moves
  RecalculateMovesDueToMove(cm);

  return this;
} // }}}

// Takes a collection of trackers and recalculates moves for their associated piece.
void ChessState::RecalculateMoves(MoveList trackers){ // {{{
  for(auto tracker_ptr : trackers){
    // Could be made more efficient by overriding createMovesForPieces
    // in order to take a PieceTracker.
    CreateMovesForPiece((*tracker_ptr)->index);
  }
} // }}}

// Recalculates possible and blocked moves for pieces affected by
// the movement of another piece.
void ChessState::RecalculateMovesDueToMove(ChessMove cm){ // {{{
  // Copy because RecalculateMoves modifies possible_moves and blocked_moves
  // so, if we run RecalculateMoves multiple times on possible_moves and
  // blocked moves we'll end up calculating things multiple times.
  auto pm_copy = possible_moves;
  auto bm_copy = blocked_moves;
  RecalculateMoves(pm_copy[cm.first]);
  RecalculateMoves(pm_copy[cm.second]);
  RecalculateMoves(bm_copy[cm.first]);
  RecalculateMoves(bm_copy[cm.second]);
} // }}}

// Removes PieceTracker pointers from possible_moves and blocked_moves
// if the object they're pointing to is a nulllptr. Also deletes
// invalidated PieceTrackers from trackers.
void ChessState::RemoveReferencesToDeadTrackers(){ // {{{
  struct is_nullptr {
    bool operator() (std::shared_ptr<PieceTracker> *& pt){
      return *pt == nullptr;
    }
  };
  for(auto & moves_collection : possible_moves){
    moves_collection.remove_if(is_nullptr());
  }
  for(auto & moves_collection : blocked_moves){
    moves_collection.remove_if(is_nullptr());
  }
  for(auto & tracker_collection : trackers){
    for(auto it = ++tracker_collection.begin(); it != tracker_collection.end(); ++it){
      it->reset();
    }
    auto second_elem_it = ++tracker_collection.begin();
    auto last_elem_it = tracker_collection.end();
    tracker_collection.erase(second_elem_it, last_elem_it);
  }
  std::cout << "End RemoveReferencesToDeadTrackers()\n";
} // }}}

// Converts move in ChessMove format to "square" format
// i.e. {0, 1} -> a8b8
std::string ChessState::chess_move_to_squares(ChessMove cm){ // {{{
  std::string move_name = "";
  move_name += index_to_square(cm.first);
  move_name += index_to_square(cm.second);
  return move_name;
} // }}}

// Converts chess board index to square
// i.e. 0 -> a8
std::string ChessState::index_to_square(char index){ // {{{
  std::string square = "";
  square += 'a' + index % 8;
  square += std::to_string(8 - index / 8);
  return square;
} // }}}

// TODO: Needs to be tested. Never been executed...
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

// Prints some information about chess state depending on attrs passed in.
void ChessState::PrintState(ChessState * cs, std::string attrs){ // {{{
  std::cout << "\nCurrent Player: " << cs->current_player << "\n";

  for(auto attr : attrs){
    switch(attr){
    // Pieces on board
    case 'p':
      std::cout << "Pieces\n";
      for(int i=0; i<NUM_SQUARES; i++){
        if(i % 8 == 0)
         std::cout << std::endl;
        std::cout << std::setw(2) << +cs->board[i] << " ";
      }
      break;
    // Ownership of pieces
    case 'o':
      std::cout << "Ownership\n";
      for(int i=0; i<NUM_SQUARES; i++){
        if(i % 8 ==0)
          std::cout << std::endl;
        std::cout << std::setw(2) << cs->ownership[i] << " ";
      }
      break;
    // Possible move counts
    case 'P':
      std::cout << "Possible Move Counts\n";
      for(int i = 0; i< NUM_SQUARES; i++){
        if(i % 8 == 0)
          std::cout << std::endl;
        std::cout << std::setw(2) << cs->possible_moves[i].size() << " ";
      }
      break;
    // Blocked move counts
    case 'B':
      std::cout << "Blocked Move Counts\n";
      for(int i = 0; i< NUM_SQUARES; i++){
        if(i % 8 == 0)
          std::cout << std::endl;
        std::cout << std::setw(2) << cs->blocked_moves[i].size() << " ";
      }
      break;
    }

    std::cout << "\n\n";
  }
}// }}}
