#include <iostream>
#include "chess/chess.h"
#include "chess/chess_state.h"
int main(){
  std::cout << "Doing stuff...\n";

  auto gs = ChessState::GetInitialState();
  ChessState * cs = static_cast<ChessState *>(gs);
  std::cout << cs->board[0][0] << std::endl;

  auto cm = ChessState::get_indices_for_move("a8h2");
  std::cout << cm.first[0] << cm.first[1] << cm.second[0] << cm.second[1] << std::endl;

  ChessState::PrintState(cs);
  auto ncs = static_cast<ChessState *>(cs->GetNewState("e2e4"));
  ChessState::PrintState(ncs);
  
  delete cs;
  delete ncs;
}
