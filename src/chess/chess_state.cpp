#include "chess_state.h"
#include <iostream>
#include <iomanip>
#include <cctype>

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

ChessState * ChessState::DeepCopy(){
  // Do that deep copy!
  // TODO: Stub.
  return new ChessState();
}

GameState<ChessMove> * ChessState::GetInitialState() {
  return GetStateFromFEN(fen::starting);
}

// TODO: Test this code. It's incomplete (I think).
void ChessState::CreateMovesForPiece(char index){
  if(index > 0){
  std::cout << "CreateMovesForPiece(" << +index << ")\n";
  char * piece = &board[index];
  if(trackers[index].size() > 0){
    auto oldpt = trackers[index].front();
    oldpt = nullptr;
  }

  auto pt = std::make_shared<PieceTracker>(piece, index);
  std::cout << "pt->index: " << +pt->index << '\n';
  if(*piece == pieces::PAWN){
    int delta = ownership[index] == player::WHITE ? deltas::UP : deltas::DOWN;
    const int move_pos = index + delta;
    // There's a piece located at delta. Add move to blocked_moves.
    // std::cout << "Piece ahead of pawn: " << +board[move_pos] << '\n';
    if(board[move_pos]){
      blocked_moves[move_pos].push_back(&pt);
    // Square is clear. Add to possible moves.
    } else {
      possible_moves[move_pos].push_back(&pt);
    }
  }
  trackers[index].push_front(pt);
  }
}

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
  // Recalculate blocked/possible moves
  std::cout << "We die here, right?\n";
  new_state->RecalculateMovesDueToMove(cm);
  std::cout << "Yup...\n";
  return new_state;
}

void ChessState::RecalculateMoves(MoveList trackers){
  for(auto tracker_ptr : trackers){
    // Could be made more efficient by overriding createMovesForPieces
    // in order to take a PieceTracker.
    std::cout << "In Recalculate moves.." << +(*tracker_ptr)->index << '\n';
    CreateMovesForPiece((*tracker_ptr)->index);
  }
}

void ChessState::RecalculateMovesDueToMove(ChessMove cm){
  RecalculateMoves(possible_moves[cm.first]);
  RecalculateMoves(possible_moves[cm.second]);
  RecalculateMoves(blocked_moves[cm.first]);
  RecalculateMoves(blocked_moves[cm.second]);
}

void ChessState::RemoveReferencesToDeadTrackers(){
  std::cout << "Begin RemoveReferencesToDeadTrackers()\n";
  struct is_nullptr {
    bool operator() (std::shared_ptr<PieceTracker> *& pt){
      return *pt == nullptr;
    }
  };
  for(auto & moves_collection : possible_moves){
    if(moves_collection.size() > 1)
    moves_collection.remove_if(is_nullptr());
  }
  for(auto & moves_collection : blocked_moves){
    moves_collection.remove_if(is_nullptr());
  }
  for(auto & tracker_collection : trackers){
    if(tracker_collection.size() > 1){
      for(auto it = ++tracker_collection.begin(); it != tracker_collection.end(); ++it){
        it->reset();
      }
      auto second_elem_it = ++tracker_collection.begin();
      auto last_elem_it = tracker_collection.end();
      tracker_collection.erase(second_elem_it, last_elem_it);
    }
  }
  std::cout << "End RemoveReferencesToDeadTrackers()\n";
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
