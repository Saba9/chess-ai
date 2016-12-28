#include <iostream>
#include "../lib/googletest/googletest/include/gtest/gtest.h"
#include "../src/chess/chess_state.h"

TEST(ChessState, GetInitialStateInitializesBoardCorrectly){
  auto cs = static_cast<ChessState *>(ChessState::GetInitialState());

  using namespace pieces;
  char royal_row[] = {
    ROOK, KNIGHT, BISHOP, KING, QUEEN, BISHOP, KNIGHT, ROOK
  };

  for(int row = 0; row < ChessState::ROW_SIZE; row++){
    for(int col = 0; col < ChessState::COL_SIZE; col++){
      int i = row * ChessState::ROW_SIZE + col;
      switch(row) {
        case 0: case 7:
          EXPECT_EQ(cs->board[i], royal_row[i % ChessState::COL_SIZE]);
          break;
        case 1: case 6:
          EXPECT_EQ(cs->board[i], PAWN);
          break;
        case 2: case 3: case 4: case 5:
          EXPECT_EQ(cs->board[i], NONE);
          break;
        default:
          FAIL() << "We shouldn't be here.";
      }
    }
  }
}
