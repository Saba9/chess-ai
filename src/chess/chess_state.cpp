#include "chess_state.h"
#include <iostream>
#include <iomanip>
#include <cctype>
#include <map>
#include <assert.h>

bool do_debug = true;
void debug(std::string out){
  if(do_debug)
    std::cout << out << '\n';
}

ChessState::~ChessState() {
  for(auto tracker_collection : trackers){
    for(auto tracker : tracker_collection){
      tracker.reset();
    }
  }
}

// Return a pointer to a deepcopy of chessstate.
// Specifically deep copies trackers, possible_moves, and blocked_moves.
ChessState * ChessState::DeepCopy(){ // {{{
  // Shallow copy
  auto copy = new ChessState(*this);
  
  /*
    Keys set equal to old pointers to piece_trackers
    values set equal to copy of those piece_trackers
  */
  typedef std::shared_ptr<PieceTracker> SharedPieceTracker;
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

GameState<ChessMove> * ChessState::GetInitialState() {
  return GetStateFromFEN(fen::starting);
}

// TODO: Test this code. It's incomplete (I think).
void ChessState::CreateMovesForPiece(char index){ // {{{
  std::cout << "CreateMovesForPiece(" << +index << ")\n";
  char * piece = &board[index];
  if(trackers[index].size() > 0){
   // trackers[index].front() = nullptr;
    for(auto & move : possible_moves[16]){
      std::cout << "What it equal? "<< +(*move).get() << '\n';
    }
  }

  trackers[index].push_front(std::make_shared<PieceTracker>(piece, index));
  auto pt = &trackers[index].front();

  if(*piece == pieces::PAWN){
    int delta = ownership[index] == player::WHITE ? deltas::UP : deltas::DOWN;
    const int move_pos = index + delta;
    // There's a piece located at delta. Add move to blocked_moves.
    // std::cout << "Piece ahead of pawn: " << +board[move_pos] << '\n';
    if(board[move_pos]){
      blocked_moves[move_pos].push_back(pt);
    // Square is clear. Add to possible moves.
    } else {
      possible_moves[move_pos].push_back(pt);
    }
  }
} // }}}

void ChessState::CreateMovesForBoard(){
  for(int i = 0; i < 64; i++){
    CreateMovesForPiece(i);
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
  cs->CreateMovesForBoard();
  return cs;
} // }}}


// Takes a move in the format of std::pair<char, char> = {old_index, new_index}
// And executes it. Does not check to see if move is legal.
// i.e. e2e4 moves white center right pawn up to spaces.
GameState<ChessMove> * ChessState::GetNewState(ChessMove cm){ // {{{
  //auto new_state = new ChessState(*this);
  auto new_state = this->DeepCopy();
  new_state->current_player = !new_state->current_player;

  auto old_location = cm.first;
  auto new_location = cm.second;

  // Move piece on board
  int piece = new_state->board[old_location];
  new_state->board[old_location] = pieces::NONE;
  new_state->board[new_location] = piece;
  // Update ownership info
  new_state->ownership[new_location] = new_state->ownership[old_location];
  // Recalculate blocked/possible moves
  new_state->RecalculateMovesDueToMove(cm);
  return new_state;
} // }}}

void ChessState::RecalculateMoves(MoveList trackers){
  for(auto tracker_ptr : trackers){
    // Could be made more efficient by overriding createMovesForPieces
    // in order to take a PieceTracker.
    CreateMovesForPiece((*tracker_ptr)->index);
  }
}

void ChessState::RecalculateMovesDueToMove(ChessMove cm){ // {{{
  std::cout << "possible cm.first" << "\n\n";
  RecalculateMoves(possible_moves[cm.first]);
  std::cout << "possible cm.second" << "\n\n";
  RecalculateMoves(possible_moves[cm.second]);
  /* Test 
  std::cout << "blocked cm.first" << "\n\n";
  RecalculateMoves(blocked_moves[cm.first]);
  std::cout << "blocked cm.second" << "\n\n";
  RecalculateMoves(blocked_moves[cm.second]);
  */
} // }}}

void ChessState::RemoveReferencesToDeadTrackers(){ // {{{
  std::cout << "Begin RemoveReferencesToDeadTrackers()\n";
  struct is_nullptr {
    bool operator() (std::shared_ptr<PieceTracker> *& pt){
      std::cout << "running is_nullptr? " << (*pt == nullptr ? "IT IS" : "NOPE" ) << std::endl;
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

void ChessState::PrintState(ChessState * cs, std::string attrs){ // {{{
  std::cout << "\nCurrent Player: " << cs->current_player << "\n";

  for(auto attr : attrs){
    switch(attr){
    // Pieces on board
    case 'p':
      std::cout << "Pieces\n";
      for(int i=0; i<64; i++){
        if(i % 8 == 0)
         std::cout << std::endl;
        std::cout << std::setw(2) << +cs->board[i] << " ";
      }
      break;
    // Ownership of pieces
    case 'o':
      std::cout << "Ownership\n";
      for(int i=0; i<64; i++){
        if(i % 8 ==0)
          std::cout << std::endl;
        std::cout << std::setw(2) << cs->ownership[i] << " ";
      }
      break;
    // Possible move counts
    case 'P':
      std::cout << "Possible Move Counts\n";
      for(int i = 0; i< 64; i++){
        if(i % 8 == 0)
          std::cout << std::endl;
        std::cout << std::setw(2) << cs->possible_moves[i].size() << " ";
      }
      break;
    // Blocked move counts
    case 'B':
      std::cout << "Blocked Move Counts\n";
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
