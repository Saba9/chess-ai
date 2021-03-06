#include <iostream>
#include <assert.h>
#include "chess/chess.h"
#include "chess/chess_state.h"
int main(){
  std::cout << "Doing stuff...\n";

  ChessState * cs = static_cast<ChessState *>(ChessState::GetInitialState());

  std::cout << "PrintState #1\n";
  ChessState::PrintState(cs, "poPB");

  auto ncs = static_cast<ChessState *>(cs->ModifyState(std::make_pair(1, 17)));
  ncs->RemoveReferencesToDeadTrackers();

  std::cout << "PrintState #2\n";
  ChessState::PrintState(ncs, "poPB");

  ncs = static_cast<ChessState *>(cs->ModifyState(std::make_pair(2, 18)));
  ncs->RemoveReferencesToDeadTrackers();

  std::cout << "PrintState #3\n";
  ChessState::PrintState(ncs, "poPB");

  std::cout << "Done doing stuff...\n";
  delete ncs;
}
