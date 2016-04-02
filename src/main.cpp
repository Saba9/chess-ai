#include <iostream>
#include "chess/chess.h"
#include "chess/chess_state.h"
int main(){
  std::cout << "Doing stuff...\n";

  auto gs = ChessState::GetInitialState();
  ChessState * cs = static_cast<ChessState *>(gs);
  std::cout << +pieces::B_QUEEN << std::endl;
  ChessState::PrintState(cs);
  auto ncs = static_cast<ChessState *>(cs->GetNewState(std::make_pair(0,16)));
  std::cout << ChessState::index_to_square(8);
  ChessState::PrintState(ncs);
  
  delete cs;
  delete ncs;
}
