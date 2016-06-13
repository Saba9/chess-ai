#include <iostream>
#include <assert.h>
#include "chess/chess.h"
#include "chess/chess_state.h"
int main(){
  std::cout << "Doing stuff...\n";

  ChessState * cs = static_cast<ChessState *>(ChessState::GetInitialState());
  std::cout << "PrintState #1\n";
  ChessState::PrintState(cs, "poPB");
  /*
  std::cout << "Sanity Check v5\n";
  std::cout << cs->trackers[8].back().get() << '\n';
  std::cout << (*cs->possible_moves[16].front()).get() << '\n';
  cs->trackers[8].back() = nullptr;
  std::cout << cs->trackers[8].back().get() << '\n';
  std::cout << (*cs->possible_moves[16].front()).get() << '\n';
  */
  std::cout << "GetNewState\n";
  auto ncs = static_cast<ChessState *>(cs->GetNewState(std::make_pair(0,16)));
  delete cs;
  std::cout << "Sanity Check v3\n";
  std::cout << "a == b? " << (ncs->trackers[8].back() == *ncs->possible_moves[16].front() ? "YES" : "NO :(") << '\n';
  std::cout << ncs->trackers[8].back().get() << '\n';
  std::cout << (*ncs->possible_moves[16].front()).get() << '\n';
  std::cout << "use count: " << ncs->trackers[8].back().use_count() << "\n";
  ncs->trackers[8].back() = nullptr;
  cs->trackers[8].back() = nullptr;
  std::cout << ncs->trackers[8].back().get() << '\n';
  std::cout << (*ncs->possible_moves[16].front()).get() << '\n';
  //assert(ncs->trackers[8].back() == nullptr);
  //assert((*ncs->possible_moves[16].front())->index == 8);
  ncs->RemoveReferencesToDeadTrackers();
  std::cout << "PrintState #2\n";
  ChessState::PrintState(ncs, "poPB");
  std::cout << "Done doing stuff...\n";
  delete ncs;
}
