#include "morse_input.h"

list<MorseInput> _LocalInput;
list<MorseInput> MorseInput::LOCAL_INPUTS = _LocalInput;

void MorseInput::addLocalInput(MorseInput input) {
  LOCAL_INPUTS.push_back(input);
}

void MorseInput::clearAllLocalInput() {
  LOCAL_INPUTS.clear();
}

list<MorseInput> MorseInput::getAllLocalInput() {
  return LOCAL_INPUTS;
}