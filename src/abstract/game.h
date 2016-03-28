#pragma once
#include <vector>
#include "game_state.h"
#include "agent.h"

template<class M>
class Game {
  public:
    Game(Agent<M>, Agent<M>);
    GameState<M> * game_state;
    void PlayGame();
    virtual std::vector<M> GetLegalMoves(GameState<M>);
    virtual Agent<M> * GetWinner(GameState<M>);
    Agent<M> * GetWinner();
    Agent<M> * a1, * a2;
  private:
    void PlayRound();
};
