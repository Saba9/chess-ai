#include <vector>

typedef std::vector<std::pair<char, char>> TwoDIndexVector;

struct PieceTracker {
  char * piece;
  char index;
  TwoDIndexVector possible_moves_indexes;
  TwoDIndexVector blocked_moves_indexes;
  
  PieceTracker(char *, char);
};