#include "game.h"
template<class M>
Game<M>::Game(Agent<M> p1, Agent<M> p2){
  this->a1 = a1;
  this->a2 = a2;
}

template<class M>
void Game<M>::PlayRound(){
  M a1_move = a1->GetMove(game_state);
  game_state = game_state->GetNewState(a1_move);
  M a2_move = a2->GetMove(game_state);
  game_state = game_state->GetNewState(a2_move);
}

template<class M>
void Game<M>::PlayGame(){
  while(GetWinner() == nullptr){
    PlayRound();
  }
}

template<class M>
Agent<M> * Game<M>::GetWinner(){
  return GetWinner(game_state);
}
