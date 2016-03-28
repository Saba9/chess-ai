#pragma once
typedef std::array<std::array<int, 8>, 8> ChessBoard;
typedef std::pair<int[2], int[2]> ChessMove;

namespace Player {
  enum player {
    WHITE,
    BLACK
  };
}

namespace pieces {
  enum pieces {
    NONE,
    W_PAWN,
    W_KNIGHT,
    W_BISHOP,
    W_ROOK,
    W_QUEEN,
    W_KING,
    B_PAWN,
    B_KNIGHT,
    B_BISHOP,
    B_ROOK,
    B_QUEEN,
    B_KING
  };
}
