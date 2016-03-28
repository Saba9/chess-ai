#include <string>
#include <array>
#include <utility>
#include "common.h"
#include "../abstract/game_state.h"
// Represent moves as string (e2e4)
class ChessState : public GameState<std::string> {
  public:
    // [0][0] is a8 [7][7] is h1
    ChessBoard board;

    static GameState<std::string> * GetInitialState();
    static ChessMove get_indices_for_move(std::string);
    static void PrintState(ChessState *);

    GameState<std::string> * GetNewState(std::string) override;
    ChessState(){};
    ~ChessState(){};
};
