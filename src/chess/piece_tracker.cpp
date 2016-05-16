#include "piece_tracker.h"
#include <iostream>
PieceTracker::PieceTracker(char * piece, char index){
  if(index < 0)
    std::cout << index << std::endl;
  this->piece = piece;
  this->index = index;
}

PieceTracker::~PieceTracker(){
}
