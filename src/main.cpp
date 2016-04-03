#include <iostream>
#include "chess/chess.h"
#include "chess/chess_state.h"
int main(){
  std::cout << "Doing stuff...\n";

  auto gs = ChessState::GetInitialState();
  ChessState * cs = static_cast<ChessState *>(gs);
  ChessState::PrintState(cs, "poP");
  auto ncs = static_cast<ChessState *>(cs->GetNewState(std::make_pair(0,16)));
  ChessState::PrintState(ncs, "poP");
  delete cs;
  delete ncs;
}
