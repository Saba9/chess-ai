#pragma once
#include "game_state.h"
template<class M> // Move
class Agent {
  public:
    virtual M getMove(GameState<M>) = 0;
};
