#include "morse_code.h"

MorseCode MorseCode::MORSE_CODE_ARRAY[] = {
  { 'A', ".-" }, { 'B', "-..." }, { 'C', "-.-." }, { 'D', "-.." }, { 'E', "." }, { 'F', "..-." }, { 'G', "--." }, { 'H', "...." }, { 'I', ".." }, { 'J', ".---" }, { 'K', "-.-" }, { 'L', ".-.." }, { 'M', "--" }, { 'N', "-." }, { 'O', "---" }, { 'P', ".--." }, { 'Q', "--.-" }, { 'R', ".-." }, { 'S', "..." }, { 'T', "-" }, { 'U', "..-" }, { 'V', "...-" }, { 'W', ".--" }, { 'X', "-..-" }, { 'Y', "-.--" }, { 'Z', "--.." }, { '1', ".----" }, { '2', "..---" }, { '3', "...--" }, { '4', "....-" }, { '5', "....." }, { '6', "-...." }, { '7', "--..." }, { '8', "---.." }, { '9', "----." }, { '0', "-----" }, { '.', ".-.-.-" }, { ',', "--..--" }, { '?', "..--.." }, { '\'', ".----." }, { '!', "-.-.--" }, { '/', "-..-." }, { '(', "-.--." }, { ')', "-.--.-" }, { '&', ".- ..." }, { ':', "---..." }, { ';', "-.-.-." }, { '=', "-...-" }, { '+', ".-.-." }, { '-', "-....-" }, { '_', "..--.-" }, { '"', ".-..-." }, { '$', "...-..-" }, { '@', ".--.-." }
};

MorseCode::MorseCode(char letter, string code) {
  this->letter = letter;
  this->code = code;
}

char MorseCode::getLetter(string val) {
  int total = sizeof(MORSE_CODE_ARRAY) / sizeof(MORSE_CODE_ARRAY[0]);
  for (int i = 0; i < total; i++) {
    if (MORSE_CODE_ARRAY[i].code == val) {
      return MORSE_CODE_ARRAY[i].letter;
    }
  }
  return '*';
}

string MorseCode::getMorseCode(char val) {
  int total = sizeof(MORSE_CODE_ARRAY) / sizeof(MORSE_CODE_ARRAY[0]);
  for (int i = 0; i < total; i++) {
    if (MORSE_CODE_ARRAY[i].letter == val) {
      return MORSE_CODE_ARRAY[i].code;
    }
  }
  return "*";
}