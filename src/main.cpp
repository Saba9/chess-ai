#include <iostream>
#include "chess/chess.h"
#include "chess/chess_state.h"
int main(){
  std::cout << "Doing stuff...\n";

  auto gs = ChessState::GetInitialState();
  ChessState * cs = static_cast<ChessState *>(gs);
  std::cout << "PrintState #1\n";
  ChessState::PrintState(cs, "poPB");
  std::cout << "GetNewState\n";
  std::cout << "address: " << cs->possible_moves[16].front() << '\n';
  auto copy = new ChessState(* cs);//static_cast<ChessState *>(cs->DeepCopy());
  ChessState::PrintState(copy, "poPB");
  std::cout << "address: " << copy->possible_moves[16].front() << '\n';
 // auto ncs = static_cast<ChessState *>(copy->GetNewState(std::make_pair(0,16)));
 // ncs->RemoveReferencesToDeadTrackers();
 /*
  std::cout << "PrintState #2\n";
  ChessState::PrintState(ncs, "poPB");
  std::cout << "Done doing stuff...\n";
*/
  delete cs;
  delete copy;
  //delete ncs;
}
