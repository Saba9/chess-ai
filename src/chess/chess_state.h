#pragma once
#include <string>
#include <array>
#include <forward_list>
#include <list>
#include <utility>
#include "common.h"
#include "../abstract/game_state.h"
class ChessState : public GameState<ChessMove> {
  public:
    // TODO: Change most of these from public to private.
    // 0 -> a8, 63 -> h1
    ChessBoard board;
    PieceTrackers trackers = {};
    Ownership ownership = {};
    MoveMatrix possible_moves;
    MoveMatrix blocked_moves;

    static ChessBoard * knight_moves;

    static GameState<ChessMove> * GetInitialState();
    static GameState<ChessMove> * GetStateFromFEN(std::string);
    static std::string chess_move_to_squares(ChessMove);
    static std::string index_to_square(char);
    static void PrintState(ChessState *, std::string);

    GameState<ChessMove> * GetNewState(ChessMove) override;

    ChessState(){};
    ~ChessState(){};

  private:
    void createMovesForPiece(char);
    void createMovesForBoard();

    void removeReferencesToDeadTrackers();
    void recalculateMovesDueToMove(ChessMove);
    void recalculateMoves(const std::list<PieceTracker *> &);

    static char potential_moves_for_rook(char);
};
