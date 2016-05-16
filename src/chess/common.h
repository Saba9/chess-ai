#pragma once
#include "piece_tracker.h"
#include <memory> // shared pointers

typedef std::array<char, 64> ChessBoard;
typedef std::pair<char, char> ChessMove;
typedef std::list<std::shared_ptr<PieceTracker> *> MoveList;
typedef std::array<MoveList, 64> MoveMatrix;
typedef std::array<std::list<std::shared_ptr<PieceTracker>>, 64> PieceTrackers;
typedef std::array<bool, 64> Ownership;

namespace helpers {
  inline void flip_bit(bool * bit){
    *bit = !*bit;
  }
}

namespace deltas {
  enum deltas : int {
    UP = -8,
    DOWN = 8,
    RIGHT = 1,
    LEFT = -1,

    D_UP_RIGHT = -7,
    D_UP_LEFT = -9,
    D_DOWN_RIGHT = 9,
    D_DOWN_LEFT = 7,

    L_UPMOST_RIGHT = -15,
    L_UP_RIGHT = -6,
    L_UPMOST_LEFT = -17,
    L_UP_LEFT = -10,

    L_DOWNMOST_RIGHT = 17,
    L_DOWN_RIGHT = 10,
    L_DOWNMOST_LEFT = 15,
    L_DOWN_LEFT = 6
  };
}

namespace rook_move {
  enum rook_move : unsigned char {
    L_UPMOST_RIGHT    = 0b00000001,
    L_UP_RIGHT        = 0b00000010,
    L_UPMOST_LEFT     = 0b00000100,
    L_UP_LEFT         = 0b00001000,
    L_DOWNMOST_RIGHT  = 0b00010000,
    L_DOWN_RIGHT      = 0b00100000,
    L_DOWNMOST_LEFT   = 0b01000000,
    L_DOWN_LEFT       = 0b10000000,
  };
}

namespace player {
  enum player : bool {
    WHITE,
    BLACK
  };
}

namespace fen {
  const std::string starting = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
}

namespace attrs {
  enum attrs : char {
    SLIDING   = 0b10000,
    DIAGONAL  = 0b01000,
    ADJACENT  = 0b00100
  };
}

namespace pieces {
  enum pieces : char {
    NONE    = 0b00000,
    PAWN    = 0b00001,
    KNIGHT  = 0b00010,
    BISHOP  = 0b11000,
    ROOK    = 0b10100,
    QUEEN   = 0b11100,
    KING    = 0b00011
  };
}
