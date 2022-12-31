#include "morse_koch.h"

#include <cstdlib>

char MorseKoch::KOCH_TRAINER[] = { 'K', 'M', 'U', 'R', 'E', 'S', 'N', 'A', 'P', 'T', 'L', 'W', 'I', '.', 'J', 'Z', '=', 'F', 'O', 'Y', ',', 'V', 'G', '5', '/', 'Q', '9', '2', 'H', '3', '8', 'B', '?', '4', '7', 'C', '1', 'D', '6', '0', 'X' };

char MorseKoch::getRandomCharByLevel(int level) {
  int maxLevel = (sizeof(KOCH_TRAINER) / sizeof(KOCH_TRAINER[0])) - 1;
  if (level >= maxLevel) {
    level = maxLevel;
  }
  if (level < 1) {
    level = 1;
  }
  int index = rand() % (level + 1);
  return KOCH_TRAINER[index];
}
