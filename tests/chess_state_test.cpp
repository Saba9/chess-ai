#include <iostream>
#include "../lib/googletest/googletest/include/gtest/gtest.h"
#include "../src/chess/chess_state.h"

/**
 * Ensures the ChessState::GetInitialState initializes board with
 * all the pieces in the correct order.
 */
TEST(ChessState, GetInitialStateInitializesBoardCorrectly){ //{{{
  auto cs = static_cast<ChessState *>(ChessState::GetInitialState());

  using namespace pieces;
  char royal_row[] = {
    ROOK, KNIGHT, BISHOP, QUEEN, KING, BISHOP, KNIGHT, ROOK
  };

  for(int row = 0; row < ChessState::ROW_SIZE; row++){
    for(int col = 0; col < ChessState::COL_SIZE; col++){
      int i = row * ChessState::ROW_SIZE + col;
      switch(row) {
        case 0: case 7:
          EXPECT_EQ(cs->board[i], royal_row[i % ChessState::COL_SIZE]); break;
        case 1: case 6:
          EXPECT_EQ(cs->board[i], PAWN); break;
        case 2: case 3: case 4: case 5:
          EXPECT_EQ(cs->board[i], NONE); break;
        default:
          FAIL() << "We shouldn't be here.";
      }
    }
  }
} //}}}

TEST(ChessState, DeepCopyReplicatesDataCorrectly){ //{{{
  auto cs_o = static_cast<ChessState *>(ChessState::GetInitialState());
  auto cs_c = cs_o->DeepCopy();

  /**
   * Ensure that all entries in `trackers` array equal each other but
   * do not point to the same spot in memory.
   */
  for(int i = 0; i < ChessState::NUM_SQUARES; i++){
    ASSERT_EQ(cs_o->trackers[i].size(), cs_c->trackers[i].size());
    auto it_pt_o = cs_o->trackers[i].begin();
    auto it_pt_c = cs_c->trackers[i].begin();
    for(; it_pt_o != cs_o->trackers[i].end(); it_pt_o++, it_pt_c++){
      EXPECT_EQ(**it_pt_o, **it_pt_c);
      EXPECT_NE(*it_pt_o, *it_pt_c);
    }
  }
  /**
   * Check to see if `possible_moves` and `blocked_moves` have the
   * same entries in both objects but do not point to the same
   * spots in memory.
   */
  for(int i = 0; i < ChessState::NUM_SQUARES; i++){
    ASSERT_EQ(cs_o->possible_moves[i].size(), cs_c->possible_moves[i].size());
    auto it_pt_o = cs_o->possible_moves[i].begin();
    auto it_pt_c = cs_c->possible_moves[i].begin();
    for(; it_pt_o != cs_o->possible_moves[i].end(); it_pt_o++, it_pt_c++){
      EXPECT_EQ(***it_pt_o, ***it_pt_c); 
      EXPECT_NE(**it_pt_o, **it_pt_c);
    }
  }
  for(int i = 0; i < ChessState::NUM_SQUARES; i++){
    ASSERT_EQ(cs_o->blocked_moves[i].size(), cs_c->blocked_moves[i].size());
    auto it_pt_o = cs_o->blocked_moves[i].begin();
    auto it_pt_c = cs_c->blocked_moves[i].begin();
    for(; it_pt_o != cs_o->blocked_moves[i].end(); it_pt_o++, it_pt_c++){
      EXPECT_EQ(***it_pt_o, ***it_pt_c); 
      EXPECT_NE(**it_pt_o, **it_pt_c);
    }
  }
} //}}}
 
TEST(ChessState, PossibleMovesCorrectInInitialStateForPawns){ //{{{ 
  auto cs = ChessState::ParseFEN(fen::starting);
  cs->CreateMovesForBoard(pieces::PAWN);
  for(int i = 0; i < ChessState::NUM_SQUARES; i++){
    int row = i / 8;
    switch(row) {
      case 0: case 1: case 6: case 7:
        EXPECT_EQ(cs->possible_moves[i].size(), 0); break;
      case 2: case 3: case 4: case 5:
        EXPECT_EQ(cs->possible_moves[i].size(), 1); break;
      default:
        FAIL() << "We shouldn't be here.";
    }
  }
} //}}}

TEST(ChessState, BlockedMovesCorrectInInitialStateForAdjacents){ //{{{
   auto cs = ChessState::ParseFEN(fen::starting);
   cs->CreateMovesForBoard(attrs::ADJACENT);

   int royal_row_counts[] = {0, 1, 1, 1, 1, 1, 1, 0};
   int pawn_row_counts[]  = {1, 0, 0, 1, 1, 0, 0, 1};
  
   for(int i = 0; i < ChessState::NUM_SQUARES; i++){
     int row = i / 8;
     int col = i % 8;
     switch(row) {
       case 0: case 7:
         EXPECT_EQ(cs->blocked_moves[i].size(), royal_row_counts[col]); break;
       case 1: case 6:
         EXPECT_EQ(cs->blocked_moves[i].size(), pawn_row_counts[col]);  break;
       case 2: case 3: case 4: case 5:
         EXPECT_EQ(cs->blocked_moves[i].size(), 0);                     break;
       DEFAULT:
         FAIL() << "We shouldn't be here.";
     }
   }
}//}}}

TEST(ChessState, BlockedMovesCorrectInInitialStateForDiagonals){ //{{{
   auto cs = ChessState::ParseFEN(fen::starting);
   cs->CreateMovesForBoard(attrs::DIAGONAL);

   int pawn_row_counts[]  = {0, 1, 1, 2, 2, 1, 1, 0};
  
   for(int i = 0; i < ChessState::NUM_SQUARES; i++){
     int row = i / 8;
     int col = i % 8;
     switch(row) {
       case 1: case 6:
         EXPECT_EQ(cs->blocked_moves[i].size(), pawn_row_counts[col]);  break;
       case 0: case 2: case 3: case 4: case 5: case 7:
         EXPECT_EQ(cs->blocked_moves[i].size(), 0);                     break;
       DEFAULT:
         FAIL() << "We shouldn't be here.";
     }
   }
}///}}}

TEST(ChessState, BlockedAndPossibleMovesCorrectInInitialStateForKnights){
  auto cs = ChessState::ParseFEN(fen::starting);
  cs->CreateMovesForBoard(pieces::KNIGHT);
  
  int possible_move_counts[64] = {
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    1, 0, 1, 0, 0, 1, 0, 1,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    1, 0, 1, 0, 0, 1, 0, 1,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0
  };

  int blocked_move_counts[64] = {
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 1, 1, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 1, 1, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0
  };

  for(int i = 0; i < ChessState::NUM_SQUARES; i++){
    EXPECT_EQ(cs->possible_moves[i].size(), possible_move_counts[i]);
    EXPECT_EQ(cs->blocked_moves[i].size(), blocked_move_counts[i]);
  }
}

TEST(ChessState, MovesCalculatedCorrectlyForLoneQueen){
  const std::string test_board = "8/8/2Q5/8/8/8/8/8 w KQkq - 0 1";
  int possible_moves_counts[8][8] = {
    {1, 0, 1, 0, 1, 0, 0, 0},
    {0, 1, 1, 1, 0, 0, 0, 0},
    {1, 1, 0, 1, 1, 1, 1, 1},
    {0, 1, 1, 1, 0, 0, 0, 0},
    {1, 0, 1, 0, 1, 0, 0, 0},
    {0, 0, 1, 0, 0, 1, 0, 0},
    {0, 0, 1, 0, 0, 0, 1, 0},
    {0, 0, 1, 0, 0, 0, 0, 1}
  };

  auto cs = ChessState::ParseFEN(test_board);
  cs->CreateMovesForBoard();

  for(int i = 0; i < ChessState::NUM_SQUARES; i++){
    int row = i / 8;
    int col = i % 8;
    EXPECT_EQ(cs->possible_moves[i].size(), possible_moves_counts[row][col]);
    EXPECT_EQ(cs->blocked_moves[i].size(), 0);
  }
}

TEST(ChessState, MovesCalculatedCorrectlyWhenBlocked){
  const std::string test_board = "8/1K6/8/3R1P2/8/5Q2/1PB5/8 w KQkq - 0 1";
  int possible_moves_counts[8][8] = {
    {1, 1, 1, 1, 0, 0, 0, 0},
    {1, 0, 1, 1, 0, 0, 0, 0},
    {1, 1, 1, 1, 0, 1, 0, 0},
    {1, 1, 1, 0, 1, 0, 0, 1},
    {1, 1, 0, 1, 2, 1, 1, 0},
    {1, 3, 1, 3, 1, 0, 1, 1},
    {0, 0, 0, 1, 1, 1, 1, 0},
    {0, 1, 0, 3, 0, 1, 0, 1}
  };

  int blocked_moves_counts[8][8] = {
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 1, 0, 3, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0}
  };

  auto cs = ChessState::ParseFEN(test_board);
  cs->CreateMovesForBoard();
  
  for(int i = 0; i < ChessState::NUM_SQUARES; i++){
    int row = i / 8;
    int col = i % 8;
    EXPECT_EQ(cs->possible_moves[i].size(), possible_moves_counts[row][col]);
    EXPECT_EQ(cs->blocked_moves[i].size() , blocked_moves_counts[row][col]);
  }
}

TEST(ChessState, MovesCalculatedCorrectlyForMultipleRooks){
  const std::string test_board = "8/1n6/5n2/8/8/2n5/6n1/8 w";
  int possible_move_counts[64] = {
    0, 0, 0, 1, 1, 0, 1, 0,
    0, 0, 0, 1, 0, 0, 0, 1,
    0, 0, 0, 1, 0, 0, 0, 0,
    1, 1, 1, 2, 0, 0, 0, 1,
    1, 0, 0, 0, 2, 1, 1, 1,
    0, 0, 0, 0, 1, 0, 0, 0,
    1, 0, 0, 0, 1, 0, 0, 0,
    0, 1, 0, 1, 1, 0, 0, 0
  };

  auto cs = ChessState::ParseFEN(test_board);
  cs->CreateMovesForBoard();

  for(int i = 0; i < ChessState::NUM_SQUARES; i++){
    EXPECT_EQ(cs->possible_moves[i].size(), possible_move_counts[i]);
    EXPECT_EQ(cs->blocked_moves[i].size(), 0);
  }
}

TEST(ChessState, PawnCapturesCalculated){
  const std::string test_board = "8/8/8/8/4p3/3P4/2P5/8";
  int possible_move_counts[64] = {
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 1, 1, 1, 0, 0, 0,
    0, 0, 1, 1, 1, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0
  };
  
  auto cs = ChessState::ParseFEN(test_board);
  cs->CreateMovesForBoard();

  for(int i = 0; i < ChessState::NUM_SQUARES; i++){
    EXPECT_EQ(cs->possible_moves[i].size(), possible_move_counts[i]) << i;
    EXPECT_EQ(cs->blocked_moves[i].size(), 0);
  }
}
