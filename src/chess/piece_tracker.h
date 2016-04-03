struct PieceTracker {
  char * piece;
  char board_index;
  std::forward_list<std::pair<char, char>> possible_moves_indexes;
  std::forward_list<std::pair<char, char>> blocked_moves_indexes;
};