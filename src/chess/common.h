#pragma once
#include "piece_tracker.h"

typedef std::array<char, 64> ChessBoard;
typedef std::pair<char, char> ChessMove;
typedef std::array<std::forward_list<char **>, 64> MoveMatrix;
typedef std::array<PieceTracker *, 64> PieceTrackers;
typedef std::array<bool, 64> Ownership;

namespace helpers {
  inline void flip_bit(bool * bit){
    *bit = !*bit;
  }
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
