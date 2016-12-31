#include <vector>

struct PieceTracker {
  char * piece;
  char index;
  
  PieceTracker(char *, char);

  bool operator==(const PieceTracker & rhs) const {
    return *piece == *rhs.piece && index == rhs.index;
  }
};
