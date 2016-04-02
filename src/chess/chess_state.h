#include <string>
#include <array>
#include <forward_list>
#include <utility>
#include "common.h"
#include "../abstract/game_state.h"
class ChessState : public GameState<ChessMove> {
  public:
    // 0 -> a8, 63 -> h1
    ChessBoard board;
    MoveMatrix moves;

    static GameState<ChessMove> * GetInitialState();
    static GameState<ChessMove> * GetStateFromFEN(std::string);
    static std::string chess_move_to_squares(ChessMove);
    static std::string index_to_square(char);
    static void PrintState(ChessState *);

    GameState<ChessMove> * GetNewState(ChessMove) override;
    
    ChessState(){};
    ~ChessState(){};
};
