#pragma once
template<class M> // Move
class GameState {
  public:
    virtual GameState<M> * GetNewState(M) = 0;
    virtual GameState<M> * ModifyState(M) = 0;
    bool current_player;
};
