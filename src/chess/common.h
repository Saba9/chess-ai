#pragma once
typedef std::array<char, 64> ChessBoard;
typedef std::pair<char, char> ChessMove;
typedef std::array<std::forward_list<char **>, 32> MoveMatrix;
typedef std::array<std::forward_list<char *>, 32> ReferenceBoard;

namespace player {
  enum player : bool {
    WHITE,
    BLACK
  };
}

namespace attrs {
  enum attrs : char {
    BLACK     = 0b100000,
    SLIDING   = 0b010000,
    DIAGONAL  = 0b001000,
    ADJACENT  = 0b000100
  };
}

namespace pieces {
  enum pieces : char {
    NONE      = 0b000000,
    W_PAWN    = 0b000001,
    W_KNIGHT  = 0b000010,
    W_BISHOP  = 0b011000,
    W_ROOK    = 0b010100,
    W_QUEEN   = 0b011100,
    W_KING    = 0b000011,
    B_PAWN    = 0b100001,
    B_KNIGHT  = 0b100010,
    B_BISHOP  = 0b111000,
    B_ROOK    = 0b110100,
    B_QUEEN   = 0b111100,
    B_KING    = 0b100011
  };
}
